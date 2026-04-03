#include "ModelLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <unordered_map>

#include <iostream>

namespace io {

    ModelLoader::ModelLoader()
        : running(true)
    {
        loaderThread = std::thread(&ModelLoader::loaderLoop, this);
    }

    ModelLoader::~ModelLoader() {
        running.store(false, std::memory_order_relaxed);
        wakeCV.notify_all();

        if (loaderThread.joinable())
            loaderThread.join();
    }

    std::unique_ptr<Model> ModelLoader::createCube(float sideLength) {
    auto model = std::make_unique<Model>("__generated_cube__");

    const float h = sideLength * 0.5f;

    using V = Vertex;

    struct Face {
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 v0, v1, v2, v3;
    };

    std::array<Face, 6> faces = {
        Face{ { 0,  0,  1}, { 1, 0, 0}, {-h,-h, h}, { h,-h, h}, { h, h, h}, {-h, h, h} },
        Face{ { 0,  0, -1}, {-1, 0, 0}, { h,-h,-h}, {-h,-h,-h}, {-h, h,-h}, { h, h,-h} },
        Face{ {-1,  0,  0}, { 0, 0, 1}, {-h,-h,-h}, {-h,-h, h}, {-h, h, h}, {-h, h,-h} },
        Face{ { 1,  0,  0}, { 0, 0,-1}, { h,-h, h}, { h,-h,-h}, { h, h,-h}, { h, h, h} },
        Face{ { 0,  1,  0}, { 1, 0, 0}, {-h, h, h}, { h, h, h}, { h, h,-h}, {-h, h,-h} },
        Face{ { 0, -1,  0}, { 1, 0, 0}, {-h,-h,-h}, { h,-h,-h}, { h,-h, h}, {-h,-h, h} }
    };

    for (const auto& face : faces) {
        uint32_t startIndex = static_cast<uint32_t>(model->vertices.size());

        glm::vec2 uv[4] = {
            {0,0}, {1,0}, {1,1}, {0,1}
        };

        glm::vec3 color = {1.0f, 1.0f, 1.0f};
        glm::vec4 tangent4 = glm::vec4(face.tangent, 1.0f);

        model->vertices.push_back({face.v0, face.normal, color, uv[0], tangent4});
        model->vertices.push_back({face.v1, face.normal, color, uv[1], tangent4});
        model->vertices.push_back({face.v2, face.normal, color, uv[2], tangent4});
        model->vertices.push_back({face.v3, face.normal, color, uv[3], tangent4});

        model->indices.insert(model->indices.end(), {
            startIndex + 0, startIndex + 1, startIndex + 2,
            startIndex + 2, startIndex + 3, startIndex + 0
        });
    }

    model->attributes = {
        static_cast<uint32_t>(model->vertices.size()),
        static_cast<uint32_t>(model->indices.size()),
        static_cast<uint32_t>(model->indices.size() / 3)
    };

    return model;
}

    void ModelLoader::requestLoad(const std::string& obj_path) {
        outstandingLoads.fetch_add(1, std::memory_order_relaxed);
        loadRequests.push(obj_path);

        wakeCV.notify_one();
    }

    bool ModelLoader::hasPendingLoads() const {
        return outstandingLoads.load(std::memory_order_acquire) > 0;
    }

    std::vector<std::unique_ptr<Model>> ModelLoader::collectLoadedModels() {
        std::vector<std::unique_ptr<Model>> modelsFound;

        auto maybeModel = loadedModels.try_pop();
        while (maybeModel.has_value()) {
            modelsFound.push_back(std::move(maybeModel.value()));

            maybeModel = loadedModels.try_pop();
        }

        return modelsFound;
    }

    void ModelLoader::loaderLoop() {
        while (running) {
            {
                std::unique_lock lock(wakeMutex);

                wakeCV.wait(lock, [&] {return !running || !loadRequests.empty(); });
            }

            std::optional<std::string> req = loadRequests.try_pop();
            if (!req.has_value())
                continue;

            struct CounterGuard {
                std::atomic<uint32_t>& c;
                ~CounterGuard() { c.fetch_sub(1, std::memory_order_release); }
            } gaurd{ outstandingLoads };

            std::unique_ptr<Model> model = std::make_unique<Model>(req.value());

            try {
                load(*model);
            }
            catch (const std::exception& e) {
                std::cerr << '\n' << e.what() << std::endl;
            }

            loadedModels.push(std::move(model));
        }
    }

    void ModelLoader::load(Model& model) {
        std::string path = model.obj_FilePath;

        Model::ModelAttributes modelAttributes{ 0, 0, 0 };

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                if (index.vertex_index >= 0 && size_t(3 * index.vertex_index + 2) < attrib.vertices.size()) {
                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0 && size_t(3 * index.normal_index + 2) < attrib.normals.size()) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0 && size_t(2 * index.texcoord_index + 1) < attrib.texcoords.size()) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }
                vertex.color = {
                    1.0f, 1.0f, 1.0f
                };
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
                    model.vertices.push_back(vertex);
                    modelAttributes.uniqueVertCount += 1;
                }
                modelAttributes.vertCount += 1;
                model.indices.push_back(uniqueVertices[vertex]);
            }
        }
        modelAttributes.trisCount = modelAttributes.vertCount / 3;

        model.attributes = modelAttributes;
    }

}

