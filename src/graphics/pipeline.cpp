#include "../include/graphics/pipeline.hpp"
#include "../include/graphics/context.hpp"
#include "../include/graphics/vertex.hpp"
#include <fstream>
#include <stdexcept>

namespace voxyl::graphics {

    Pipeline::Pipeline(const Context& context, const std::string& vertex, const std::string& fragment, const State& state)
        : core(context.device()), pipeline(VK_NULL_HANDLE), shape(state.layout) {
        build(vertex, fragment, state);
    }

    Pipeline::~Pipeline() {
        if (core != VK_NULL_HANDLE && pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(core, pipeline, nullptr);
        }
    }

    void Pipeline::bind(const VkCommandBuffer_T* buffer) const {
        vkCmdBindPipeline(const_cast<VkCommandBuffer>(buffer), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }

    void Pipeline::solid(State& state) {
        state.assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        state.assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        state.assembly.primitiveRestartEnable = VK_FALSE;

        state.raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        state.raster.depthClampEnable = VK_FALSE;
        state.raster.rasterizerDiscardEnable = VK_FALSE;
        state.raster.polygonMode = VK_POLYGON_MODE_FILL;
        state.raster.lineWidth = 1.0f;
        state.raster.cullMode = VK_CULL_MODE_BACK_BIT;
        state.raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        state.raster.depthBiasEnable = VK_FALSE;
    }

    void Pipeline::flat(State& state) {
        state.assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        state.assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        state.assembly.primitiveRestartEnable = VK_FALSE;

        state.raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        state.raster.depthClampEnable = VK_FALSE;
        state.raster.rasterizerDiscardEnable = VK_FALSE;
        state.raster.polygonMode = VK_POLYGON_MODE_FILL;
        state.raster.lineWidth = 1.0f;
        state.raster.cullMode = VK_CULL_MODE_NONE;
        state.raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        state.raster.depthBiasEnable = VK_FALSE;
    }

    std::vector<char> Pipeline::read(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    void Pipeline::build(const std::string& vertex, const std::string& fragment, const State& state) {
        std::vector<char> vertCode = read(vertex);
        std::vector<char> fragCode = read(fragment);

        VkShaderModule head;
        VkShaderModule tail;

        compile(vertCode, &head);
        compile(fragCode, &tail);

        VkPipelineShaderStageCreateInfo vertStageInfo{};
        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = head;
        vertStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragStageInfo{};
        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = tail;
        fragStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkGraphicsPipelineCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.stageCount = 2;
        info.pStages = shaderStages;
        info.pVertexInputState = &vertexInputInfo;
        info.pInputAssemblyState = &state.assembly;
        info.pViewportState = &state.viewport;
        info.pRasterizationState = &state.raster;
        info.pMultisampleState = &state.multisample;
        info.pColorBlendState = &state.blend;
        info.pDepthStencilState = &state.depth;
        info.layout = state.layout;
        info.renderPass = state.pass;
        info.subpass = state.subpass;

        VkPipelineDynamicStateCreateInfo dynamic{};
        if (!state.states.empty()) {
            dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamic.dynamicStateCount = static_cast<uint32_t>(state.states.size());
            dynamic.pDynamicStates = state.states.data();
            info.pDynamicState = &dynamic;
        }

        if (vkCreateGraphicsPipelines(core, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
            vkDestroyShaderModule(core, tail, nullptr);
            vkDestroyShaderModule(core, head, nullptr);
            throw std::runtime_error("Graphics pipeline execution generation failed");
        }

        vkDestroyShaderModule(core, tail, nullptr);
        vkDestroyShaderModule(core, head, nullptr);
    }

    void Pipeline::compile(const std::vector<char>& code, VkShaderModule* module) const {
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(core, &info, nullptr, module) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

}