#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

template<typename T>
AGPTracer::Entities::MeshGeometry_t<T>::MeshGeometry_t(const std::filesystem::path& filename) {
    const std::filesystem::path ext = filename.extension();
    if (ext == ".obj") {
        readObj(filename);
    }
    else if (ext == ".su2") {
        readSU2(filename);
    }
    else {
        std::cerr << "Error: Mesh geometry unstructured unknown file extension: '" << ext << "'. Exiting." << std::endl;
        exit(72);
    }
}

template<typename T>
auto AGPTracer::Entities::MeshGeometry_t<T>::readObj(const std::filesystem::path& filename) -> void {
    size_t nv  = 0;
    size_t nvt = 0;
    size_t nvn = 0;
    size_t nf  = 0;
    unsigned int nsides;
    std::string line;
    std::string dummy;

    std::ifstream meshfile(filename);
    if (!meshfile.is_open()) {
        std::cerr << "Error: file '" << filename << "' could not be opened. Exiting." << std::endl;
        exit(101);
    }

    // Getting number of elements
    while (std::getline(meshfile, line)) {
        if (line.length() < 3) {
            continue;
        }

        if (line[0] == 'v' && line[1] == ' ') {
            ++nv;
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            ++nvt;
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            ++nvn;
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream liness(line);
            liness >> dummy;

            nsides = 0;
            while (liness >> dummy) {
                ++nsides;
            }
            nf += nsides - 2;
        }
    }

    // Getting normals, vertex coordinates and texture coordinates
    size_t v_counter  = 0;
    size_t vt_counter = 0;
    size_t vn_counter = 0;
    T val0;
    T val1;
    T val2;

    nodes_               = std::vector<AGPTracer::Entities::Vec3<T>>(nv);
    texture_coordinates_ = std::vector<std::array<T, 2>>(nvt);
    normals_             = std::vector<AGPTracer::Entities::Vec3<T>>(nvn);

    meshfile.clear();
    meshfile.seekg(0, std::ios::beg);

    while (std::getline(meshfile, line)) {
        if (line.length() < 3) {
            continue;
        }

        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream liness(line);

            liness >> dummy >> val0 >> val1 >> val2;
            nodes_[v_counter] = AGPTracer::Entities::Vec3<T>(val0, val1, val2);
            ++v_counter;
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            std::istringstream liness(line);

            liness >> dummy >> val0 >> val1;
            texture_coordinates_[vt_counter] = {val0, val1};
            ++vt_counter;
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            std::istringstream liness(line);

            liness >> dummy >> val0 >> val1 >> val2;
            normals_[vn_counter] = AGPTracer::Entities::Vec3<T>(val0, val1, val2);
            ++vn_counter;
        }
    }

    // Filling faces
    size_t f_counter = 0;
    std::string material;
    std::array<std::string, 3> tokens;
    std::string value;
    size_t pos;

    const size_t n_tris       = nf;
    mat_                      = std::vector<std::string>(n_tris);
    face_nodes_               = std::vector<std::array<size_t, 3>>(n_tris);
    face_texture_coordinates_ = std::vector<std::array<size_t, 3>>(n_tris);
    face_normals_             = std::vector<std::array<size_t, 3>>(n_tris);
    std::vector<std::array<bool, 3>> missing_normals(n_tris);
    std::vector<std::array<bool, 3>> missing_texture_coordinates(n_tris);

    meshfile.clear();
    meshfile.seekg(0, std::ios::beg);

    while (std::getline(meshfile, line)) {
        if (line.length() < 3) {
            continue;
        }

        if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream liness(line);
            liness >> dummy >> tokens[0] >> tokens[1];
            while (liness >> tokens[2]) {
                for (unsigned int i = 0; i < 3; ++i) {
                    value           = tokens[i];
                    mat_[f_counter] = material;
                    pos             = value.find('/');
                    if (pos == std::string::npos) {
                        const long long node_obj_index = std::stoll(value, nullptr);
                        const size_t node_index        = (node_obj_index > 0) ? node_obj_index - 1 : nodes_.size() + node_obj_index;

                        face_nodes_[f_counter][i]                 = node_index;
                        face_texture_coordinates_[f_counter][i]   = static_cast<size_t>(-1);
                        face_normals_[f_counter][i]               = static_cast<size_t>(-1);
                        missing_normals[f_counter][i]             = true;
                        missing_texture_coordinates[f_counter][i] = true;
                    }
                    else {
                        const long long node_obj_index = std::stoll(value.substr(0, pos), nullptr);
                        const size_t node_index        = (node_obj_index > 0) ? node_obj_index - 1 : nodes_.size() + node_obj_index;

                        face_nodes_[f_counter][i] = node_index;
                        value.erase(0, pos + 1);

                        pos = value.find('/');
                        if (pos == std::string::npos) {
                            const long long texture_coordinate_obj_index = std::stoll(value, nullptr);
                            const size_t texture_coordinate_index = (texture_coordinate_obj_index > 0) ? texture_coordinate_obj_index - 1 : texture_coordinates_.size() + texture_coordinate_obj_index;

                            face_texture_coordinates_[f_counter][i]   = texture_coordinate_index;
                            face_normals_[f_counter][i]               = static_cast<size_t>(-1);
                            missing_normals[f_counter][i]             = true;
                            missing_texture_coordinates[f_counter][i] = false;
                        }
                        else {
                            if (pos == 0) {
                                face_texture_coordinates_[f_counter][i]   = static_cast<size_t>(-1);
                                missing_texture_coordinates[f_counter][i] = true;
                            }
                            else {
                                const long long texture_coordinate_obj_index = std::stoll(value.substr(0, pos), nullptr);
                                const size_t texture_coordinate_index =
                                    (texture_coordinate_obj_index > 0) ? texture_coordinate_obj_index - 1 : texture_coordinates_.size() + texture_coordinate_obj_index;

                                face_texture_coordinates_[f_counter][i]   = texture_coordinate_index;
                                missing_texture_coordinates[f_counter][i] = false;
                            }
                            value.erase(0, pos + 1);

                            const long long normal_obj_index = std::stoll(value, nullptr);
                            const size_t normal_index        = (normal_obj_index > 0) ? normal_obj_index - 1 : normals_.size() + normal_obj_index;

                            face_normals_[f_counter][i]   = normal_index;
                            missing_normals[f_counter][i] = false;
                        }
                    }
                }
                ++f_counter;
                tokens[1] = tokens[2];
            }
        }
        else if (line.length() > 7 && line[0] == 'u' && line[1] == 's' && line[2] == 'e' && line[3] == 'm' && line[4] == 't' && line[5] == 'l' && line[6] == ' ') { // check if there is :
            std::istringstream liness(line);
            liness >> dummy >> tokens[0];
            pos = tokens[0].find(':');
            if (pos != std::string::npos) {
                tokens[0].erase(0, pos + 1);
            }
            material = tokens[0];
        }
    }

    meshfile.close();

    build_missing_normals(missing_normals);
    build_missing_texture_coordinates(missing_texture_coordinates);
}

template<typename T>
auto AGPTracer::Entities::MeshGeometry_t<T>::readSU2(const std::filesystem::path& filename) -> void {
    size_t nv = 0;
    size_t nf = 0;
    std::string line;
    std::string token;
    unsigned int value;
    bool wall_started = false;

    std::ifstream meshfile(filename);
    if (!meshfile.is_open()) {
        std::cerr << "Error: file '" << filename << "' could not be opened. Exiting." << std::endl;
        exit(102);
    }

    // Getting number of elements
    while (std::getline(meshfile, line)) {
        std::istringstream liness(line);
        liness >> token;

        if (token == "NPOIN=") {
            liness >> value;
            wall_started = false;
            nv += value;
        }
        else if (token == "MARKER_TAG=") {
            liness >> token;
            if (token == "WALL") {
                wall_started = true;
                std::getline(meshfile, line);
            }
            else {
                wall_started = false;
            }
        }
        else if (token.empty()) {
            wall_started = false;
        }
        else if (token == "5") {
            if (wall_started) {
                ++nf;
            }
        }
        else if (token == "9") {
            if (wall_started) {
                nf += 2;
            }
        }
    }

    // Getting normals, vertex coordinates and texture coordinates
    size_t v_counter = 0;
    T val0;
    T val1;
    T val2;
    bool points_started = false;

    nodes_ = std::vector<AGPTracer::Entities::Vec3<T>>(nv);

    meshfile.clear();
    meshfile.seekg(0, std::ios::beg);

    while (std::getline(meshfile, line)) {
        std::istringstream liness(line);
        if ((line == "\r") || (line.empty())) {
            token = "";
        }
        else {
            liness >> token;
        }

        if (token == "NELEM=" || token == "NMARK=" || token == "MARKER_TAG=") {
            points_started = false;
        }
        else if (token == "NPOIN=") {
            points_started = true;
            std::getline(meshfile, line);
        }

        if (points_started && (!token.empty())) {
            std::istringstream liness2(line);
            liness2 >> val0 >> val1 >> val2;
            nodes_[v_counter] = AGPTracer::Entities::Vec3<T>(val0, val1, val2);
            ++v_counter;
        }
    }

    // Filling faces
    size_t f_counter = 0;
    std::string material;
    std::array<size_t, 3> tokens;
    wall_started = false;
    std::string marker_token;

    const size_t n_tris       = nf;
    mat_                      = std::vector<std::string>(n_tris);
    normals_                  = std::vector<AGPTracer::Entities::Vec3<T>>(3 * n_tris);
    texture_coordinates_      = std::vector<std::array<T, 2>>(3 * n_tris);
    face_nodes_               = std::vector<std::array<size_t, 3>>(n_tris);
    face_texture_coordinates_ = std::vector<std::array<size_t, 3>>(n_tris);
    face_normals_             = std::vector<std::array<size_t, 3>>(n_tris);

    meshfile.clear();
    meshfile.seekg(0, std::ios::beg);

    while (std::getline(meshfile, line)) {
        std::istringstream liness(line);
        if ((line == "\r") || (line.empty())) {
            token = "";
        }
        else {
            liness >> token;
        }

        if (token == "MARKER_TAG=") {
            liness >> marker_token;
            if (marker_token != "WALL") {
                wall_started = false;
            }
        }
        else if (token.empty() || token == "NMARK=" || token == "NELEM=" || token == "MARKER_ELEMS=") {
            wall_started = false;
        }

        if (wall_started) {
            if (token == "5") {
                for (unsigned int i = 0; i < 3; ++i) {
                    liness >> tokens[i];
                    face_nodes_[f_counter][i]               = tokens[i];
                    mat_[f_counter]                         = material;
                    normals_[3 * f_counter + i]             = AGPTracer::Entities::Vec3<T>(0.0); // CHECK fill
                    texture_coordinates_[3 * f_counter + i] = std::array<T, 2>{0.0, 0.0};
                    face_texture_coordinates_[f_counter][i] = 3 * f_counter + i;
                    face_normals_[f_counter][i]             = 3 * f_counter + i;
                }
                ++f_counter;
            }
            else if (token == "9") {
                for (unsigned int i = 0; i < 3; ++i) {
                    liness >> tokens[i];
                    face_nodes_[f_counter][i]               = tokens[i];
                    mat_[f_counter]                         = material;
                    normals_[3 * f_counter + i]             = AGPTracer::Entities::Vec3<T>(0.0); // CHECK fill
                    texture_coordinates_[3 * f_counter + i] = std::array<T, 2>{0.0, 0.0};
                    face_texture_coordinates_[f_counter][i] = 3 * f_counter + i;
                    face_normals_[f_counter][i]             = 3 * f_counter + i;
                }
                ++f_counter;
                tokens[1] = tokens[2];
                liness >> tokens[2];
                for (unsigned int i = 0; i < 3; ++i) {
                    face_nodes_[f_counter][i]               = tokens[i];
                    mat_[f_counter]                         = material;
                    normals_[3 * f_counter + i]             = AGPTracer::Entities::Vec3<T>(0.0); // CHECK fill
                    texture_coordinates_[3 * f_counter + i] = std::array<T, 2>{0.0, 0.0};
                    face_texture_coordinates_[f_counter][i] = 3 * f_counter + i;
                    face_normals_[f_counter][i]             = 3 * f_counter + i;
                }
                ++f_counter;
            }
        }

        if (token == "MARKER_TAG=") {
            if (marker_token == "WALL") {
                wall_started = true;
                std::getline(meshfile, line);
            }
        }
    }

    meshfile.close();
}

template<typename T>
auto AGPTracer::Entities::MeshGeometry_t<T>::build_missing_normals(const std::vector<std::array<bool, 3>>& normals_to_build) -> void {
    size_t normals_to_add = 0;
    for (size_t i = 0; i < normals_to_build.size(); ++i) {
        for (size_t j = 0; j < normals_to_build[i].size(); ++j) {
            if (normals_to_build[i][j]) {
                ++normals_to_add;
                break;
            }
        }
    }

    if (normals_to_add > 0) {
        const size_t offset = normals_.size();
        normals_.resize(offset + normals_to_add);

        size_t index = 0;
        for (size_t i = 0; i < normals_to_build.size(); ++i) {
            bool normal_to_add = false;
            for (size_t j = 0; j < normals_to_build[i].size(); ++j) {
                if (normals_to_build[i][j]) {
                    normal_to_add = true;
                    break;
                }
            }
            if (normal_to_add) {
                normals_[offset + index] = (nodes_[face_nodes_[i][1]] - nodes_[face_nodes_[i][0]]).cross(nodes_[face_nodes_[i][2]] - nodes_[face_nodes_[i][0]]).normalize_inplace();
                for (size_t j = 0; j < normals_to_build[i].size(); ++j) {
                    if (normals_to_build[i][j]) {
                        face_normals_[i][j] = offset + index;
                    }
                }
                ++index;
            }
        }
    }
}

template<typename T>
auto AGPTracer::Entities::MeshGeometry_t<T>::build_missing_texture_coordinates(const std::vector<std::array<bool, 3>>& texture_coordinates_to_build) -> void {
    bool texture_coordinates_to_add = false;
    for (size_t i = 0; i < texture_coordinates_to_build.size(); ++i) {
        for (size_t j = 0; j < texture_coordinates_to_build[i].size(); ++j) {
            if (texture_coordinates_to_build[i][j]) {
                texture_coordinates_to_add = true;
                break;
            }
        }
    }

    if (texture_coordinates_to_add) {
        texture_coordinates_.emplace_back(std::array<T, 2>{0.0, 0.0});
        for (size_t i = 0; i < texture_coordinates_to_build.size(); ++i) {
            for (size_t j = 0; j < texture_coordinates_to_build[i].size(); ++j) {
                if (texture_coordinates_to_build[i][j]) {
                    face_texture_coordinates_[i][j] = texture_coordinates_.size() - 1;
                }
            }
        }
    }
}
