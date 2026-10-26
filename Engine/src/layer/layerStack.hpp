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
            std::vector<Layer*>::reverse_iterator rbegin(){ return m_Layers.rbegin();}
            std::vector<Layer*>::reverse_iterator rend(){ return m_Layers.rend();}
        private:
            std::vector<Layer*> m_Layers;
            unsigned int m_LayerInsertIndex = 0;
    };
}