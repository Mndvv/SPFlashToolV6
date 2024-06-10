#include "version.h"
#include <sstream>

#include "../Utility/FileUtils.h"
#include "version_def.h"

template<typename T>
std::string HexToString(T hex) {   //Usage: HexToString ( Hex );
    std::ostringstream ss;
    ss <<"0x"<< std::hex<<hex;
    return ss.str();
}

template<typename T>
T StringToNumber(const std::string &text) {  //Usage: StringToNumber<Type> ( String );
    std::istringstream ss(text);
    T result;
    return ss >> result ? result : 0;
}

bool ToolInfo::IsCustomerVer(){
    return CUSTOMER_VER;
}

bool ToolInfo::IsTempVer(){
    return TEMP_VER;
}

std::string ToolInfo::ToolName() {
    char toolName[256] = { 0 };
    sprintf(toolName, "%s v%d.%d.0%d", APP_NAME.toStdString().c_str(), MAJOR_VER, MINOR_VER,
            BUILD_NUM);
    return std::string(toolName);
}

std::string ToolInfo::BuildInfo() {
    return std::string(BUILD_DATE).append(" (")
            .append(HexToString(REVISION_NUM)).append(")");
}

std::string ToolInfo::Comments() {
    return std::string(COMMENTS);
}
// TODO should re-generate GetModuleFileName.
std::string ToolInfo::LoadPath() {

   return FileUtils::GetAppDirectory();
}

std::string ToolInfo::ToString() {
    return std::string(
        "==================================================================================================================================\n")
        .append(ToolName()).append(" runtime trace is ON.\n") .append(
        "Build: ").append(BuildInfo()).append("\n").append(
        "Comment: ").append(COMMENTS).append("\n").append(
            "Load Path: ").append(LoadPath()) .append(
        "\n==================================================================================================================================\n");
}

std::string ToolInfo::VersionNum()
{
    char ver[20] = {0};

    sprintf(ver, "%d.%d.0%d.sn%d", MAJOR_VER, MINOR_VER, BUILD_NUM, BUILD_SN);

    return std::string(ver);
}
