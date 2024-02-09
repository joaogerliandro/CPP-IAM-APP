#include <common.h>

int main()
{
    try
    {
        LDAP *ldap = ldap_init("ldap:/localhost", LDAP_PORT);

        if (ldap == nullptr) 
        {
            throw std::runtime_error("Failed to connect to LDAP Server.");
        }

        ldap_unbind(ldap);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}