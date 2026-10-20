#include "pseudoLatex/psFile.hpp"


namespace scivibe{
    bool PsFile::Load(const std::string& filePath){
        std::ifstream file(filePath);
        if(!file.is_open()){
            SCIVIBE_CORE_INFO("Le fichier {0} ne s'est pas ouvert", filePath);
            return false;
        } 
        m_LastSaved = std::filesystem::last_write_time(filePath);
        m_HasChanged = false;
        std::stringstream buffer;
        buffer<<file.rdbuf();
        std::string content = buffer.str();
        m_Paragraphs.clear();
        const std::string delimiter = "#par";
        size_t start = 0;
        size_t end = 0;
        while ((end = content.find(delimiter,start)) != std::string::npos){
            m_Paragraphs.push_back(content.substr(start,end-start));
            start = end + delimiter.length();
        }
        if (start < content.size()) {
            m_Paragraphs.push_back(content.substr(start));
        }
        m_HasChanged = false;
        m_Index = 0;
        return true;
    }
    const std::string& PsFile::GetParagraph() const{
        return GetParagraph(m_Index);
    }
    const std::string& PsFile::GetParagraph(size_t index) const{
        if(index >= m_Paragraphs.size()){
            SCIVIBE_CORE_ERROR("Indice de paragraphe {0} invalide : {1} paragraphes disponibles", index, m_Paragraphs.size());
        }
        return m_Paragraphs.at(index);
    }
    size_t PsFile::GetParagraphCount() const{
        return m_Paragraphs.size();
    }
    size_t PsFile::GetParagraphIndex() const{
        return m_Index;
    }
    bool PsFile::HasChanged(const std::string& filePath){
        m_HasChanged =  m_LastSaved != std::filesystem::last_write_time(filePath);
        return m_HasChanged;
    }
    /* retourne vrai si le fichier a changé et update le texte à afficher */
    bool PsFile::Update(const std::string& filePath){
        if(HasChanged(filePath)){
            Load(filePath);
            return true;
        }
        return false;
    }
    void PsFile::NextParagraph(){
        if(!m_Paragraphs.empty() && m_Index < m_Paragraphs.size() - 1) ++m_Index;
    }
    void PsFile::PreviousParagraph(){
        if(m_Index > 0) --m_Index;
    }
    void PsFile::Reset(){
        m_Index = 0;
    }
}
