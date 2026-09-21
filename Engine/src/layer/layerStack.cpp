#include "layer/layerStack.hpp"

namespace scivibe{
        LayerStack::LayerStack(){
        }

        LayerStack::~LayerStack(){
            for(Layer* layer : m_Layers){
                layer->OnDetach();
                delete layer;
            }
        }

        void LayerStack::PushLayer(Layer* layer){
            m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex ,layer);
            ++m_LayerInsertIndex;
        }
        void LayerStack::PushOverLayer(Layer* overlay){
            m_Layers.emplace_back(overlay);

        }
        void LayerStack::PopLayer(Layer* layer){
            auto it = std::find(m_Layers.begin(), m_Layers.end(),layer);
            if(it!= m_Layers.end()){
                m_Layers.erase(it);
                --m_LayerInsertIndex; // ici ca marche aussi non ?
            }
        }
        void LayerStack::PopOverLayer(Layer* overlay){
            auto it = std::find(m_Layers.begin(), m_Layers.end(),overlay);
            if(it!= m_Layers.end()){
                m_Layers.erase(it);
            }
        }

}
