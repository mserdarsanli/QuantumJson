#include <ios>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

int main()
{
    std::cin.exceptions( std::ios::badbit );
    std::cout.exceptions( std::ios::badbit );

    std::string in( std::istreambuf_iterator< char >( std::cin ),
                    std::istreambuf_iterator< char >() );

    std::cout << "#include <array>\n"
              << "\n"
              << "std::array< unsigned char, " << in.size() << " > libCommonData = {\n";

    for ( size_t i = 0; i < in.size(); ++i )
    {
        if ( i % 12 == 0 )
        {
            std::cout << "   ";
        }

        std::cout << " 0x" << std::hex << std::setfill( '0' ) << std::setw( 2 ) << (int)(unsigned char)in[ i ] << ",";

        if ( i % 12 == 11 || i == in.size() - 1 )
        {
            std::cout << "\n";
        }
    }

    std::cout << "};\n";

    return 0;
}
