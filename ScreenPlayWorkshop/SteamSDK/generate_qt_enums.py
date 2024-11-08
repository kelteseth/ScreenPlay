import json
import os.path

def createEnum(name,valueList):
    enum_string_values = "\n"
    
    for value in valueList:
        # QML enums must start with uppercase Qt < 6
        value_name =  value["name"]
        value_name  = value_name[0].upper()  + value_name[1:]
        enum_string_values += "\t" + value_name + " = " + value["value"]
        enum_string_values += ",\n"

    enum_string = "enum class "
    enum_string += name
    enum_string += "\n{"
    enum_string += "\t"+ enum_string_values
    enum_string += " };"
    enum_string += "\nQ_ENUM(" + name +")"
    enum_string += "\n\n"
    return enum_string
    

if __name__ == '__main__':

    print("Update enums") 
    my_path = os.path.abspath(os.path.dirname(__file__))
    path = os.path.join(my_path, "public/steam/steam_api.json")

    file_begin = """// GENERATED FILE DO NOT EDIT
// TO REGENERATE:
// > python.exe generate_qt_enums.py
// Copy to ScreenPlayUtil/inc/public/ScreenPlayUtil/steamenumsgenerated.h

#pragma once
#include <QObject>
#include <QQmlEngine>

namespace ScreenPlayWorkshop {

class Steam : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    explicit Steam();

    """

    file_end = """
};
}
    """

    with open(path) as json_file:
        data = json.load(json_file)
        enum_list = data["enums"]

        list = []
        file_content = ""
        for enumObj in enum_list:
            file_content += createEnum(enumObj["enumname"],enumObj["values"])
        
        file_complete  = file_begin
        file_complete  += file_content
        file_complete  += file_end

    with open('steamenumsgenerated.h', 'w') as outfile:
        outfile.write(file_complete)

    print("Update write") 


