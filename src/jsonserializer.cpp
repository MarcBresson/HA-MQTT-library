#include <HAMqtt.h>

String quote(String value, String quotation_mark = "\""){
    String s = quotation_mark;
    s += value;
    return s + quotation_mark;
}

String serializerList(std::vector<String> array, bool enclose){
    String s;
    if(enclose){
        s = "[";
    } else {
        s = "";
    }

    for(uint16_t i = 0; i < array.size(); i++){
        if(array[i][0] == '{' || array[i][0] == '(' || array[i][0] == '[' || array[i] == "false" || array[i] == "true"){
            s += array[i];
        } else {
            s += quote(array[i], "\"");
        }
        s += ",";
    }

    // replace the last comma
    if(enclose){
        s.setCharAt(s.length() - 1, ']');
    } else {
        s.substring(0, s.length() - 1);
    }

    return s;
}

String serializerDict(std::vector<Dict> dictionnary, bool enclose){
    String s;
    if(enclose){
        s = "{";
    } else {
        s = "";
    }

    for(uint16_t i = 0; i < dictionnary.size(); i++){
        s += serializerKeyValue(dictionnary[i].key, dictionnary[i].value);
        s += ",";
    }

    // replace the last comma
    if(enclose){
        s.setCharAt(s.length() - 1, '}');
    } else {
        s = s.substring(0, s.length() - 1);
    }
    return s;
}

String serializerKeyValue(String key, String value){
    String key_json;
    key_json = quote(key);

    String value_json;
    if(value[0] == '{' || value[0] == '(' || value[0] == '[' || value == "false" || value == "true"){
        value_json = value;
    } else {
        value_json = quote(value);
    }

    return key_json + ":" + value_json;
}
