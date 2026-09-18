#pragma once

#include "core/core.hpp"
#include "layer/layer.hpp"

#include <vector>

namespace scivibe{
    class SCIVIBE_API LayerStack
    {
        public: 
            LayerStack();
            ~LayerStack();

            void PushLayer(Layer* layer);
            void PushOverLayer(Layer* overlay);
            void PopLayer(Layer* layer);
            void PopOverLayer(Layer* overlay);

            std::vector<Layer*>::iterator begin(){ return m_Layers.begin();}
            std::vector<Layer*>::iterator end(){ return m_Layers.end();}
        private:
            std::vector<Layer*> m_Layers;
            std::vector<Layer*>::iterator m_LayerInsert;
    };
}