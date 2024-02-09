#include <common.h>

bool phonenumber_isvalid(const std::string& str) 
{
    return std::regex_match(str, std::regex("\\d+"));
}

void add_user(LDAP* ld, const std::string& full_name, const std::string& login, const std::string& phone_number, const std::vector<std::string>& group_list) 
{
    // Implemente aqui a lógica para adicionar o usuário ao Active Directory
    // Utilize as classes e atributos LDAP adequados
}

// Função para adicionar um grupo ao Active Directory
void add_group(LDAP* ld, const std::string& identifier, const std::string& description) 
{
    // Implemente aqui a lógica para adicionar o grupo ao Active Directory
    // Utilize as classes e atributos LDAP adequados
}

void process_xml(LDAP* ldap, const std::string& file_name) 
{
    pugi::xml_document xml_doc;
    if (!xml_doc.load_file(file_name.c_str())) 
    {
        throw std::runtime_error("Error opening XML file: " + file_name);
    }

    pugi::xml_node root_node = xml_doc.first_child();
    if (!root_node) 
    {
        throw std::runtime_error("XML file is empty: " + file_name);
    }

    for (pugi::xml_node attr_node = root_node.child("add"); attr_node; attr_node = attr_node.next_sibling("add")) 
    {
        std::string class_name = attr_node.attribute("class-name").value();
        if (class_name == "Usuario") 
        {
            std::string full_name, login, phonenumber;
            std::vector<std::string> group_list;

            for (pugi::xml_node child_node = attr_node.first_child(); child_node; child_node = child_node.next_sibling()) 
            {
                std::string attr_name = child_node.attribute("attr-name").value();
                std::string attr_value = child_node.child_value();

                if (attr_name == "Nome Completo")
                    full_name = attr_value;
                else if (attr_name == "Login")
                    login = attr_value;
                else if (attr_name == "Telefone") 
                {
                    phonenumber = std::regex_replace(attr_value, std::regex("\\D+"), "");
                }
                else if (attr_name == "Grupo")
                    group_list.push_back(attr_name);
            }

            if (!full_name.empty() && !login.empty() && !phonenumber.empty() && phonenumber_isvalid(phonenumber))
                add_user(ldap, full_name, login, phonenumber, group_list);
            else
                throw std::runtime_error("Ivalid User in XML file: " + file_name);
        }
        else if (class_name == "Grupo") 
        {
            std::string identifier;
            std::string description;

            for (pugi::xml_node child_node = attr_node.first_child(); child_node; child_node = child_node.next_sibling()) 
            {
                std::string attr_name = child_node.attribute("attr-name").value();
                std::string attr_value = child_node.child_value();

                if (attr_name == "Identificador")
                    identifier = attr_value;
                else if (attr_name == "Descricao")
                    description = attr_value;
            }

            if (!identifier.empty() && !description.empty())
                add_group(ldap, identifier, description);
            else
                throw std::runtime_error("Invalid Group in XML file: " + file_name);
        }
    }
}

int main()
{
    try
    {
        LDAP *ldap = ldap_init("ldap:/localhost", LDAP_PORT);

        if (ldap == nullptr) 
        {
            throw std::runtime_error("Failed to init LDAP Server connection.");
        }

        LDAP_RETCODE result = (LDAP_RETCODE) ldap_simple_bind_s(ldap, NULL, NULL);
        
        if (result != LDAP_SUCCESS) 
        {
            throw std::runtime_error("Failed to connect to LDAP Server.");
        }

        process_xml(ldap, "AddGrupo1.xml");
        process_xml(ldap, "AddGrupo2.xml");
        process_xml(ldap, "AddGrupo3.xml");
        process_xml(ldap, "AddUsuario1.xml");
        process_xml(ldap, "ModifyUsuario.xml");

        ldap_unbind(ldap);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}