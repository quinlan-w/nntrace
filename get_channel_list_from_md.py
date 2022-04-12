

if __name__ == "__main__":
    file = "channel_list.md"
    c_file = open("temp", "w")
    f = open(file)
    lines = f.readlines()
    for line in lines[2:-1]:
        new_line =line.replace("单元", "/").replace("通道", "/").replace("累加", "count")
        _, channel_id, _signal, description, _ = new_line.split("|")
        eu_name, sub_name, _ = description.split("/")
        eu_id = eu_name.upper()
        eu_type = eu_name.upper()[0:-1]
        name = eu_name+"_"+sub_name
        wline = "{" + channel_id.strip() + ", " + eu_id + ", " + eu_type + ", " + "\"" + name.strip() + "\"" + "}" + ",\n"
        print(wline)
        c_file.write(wline)
    c_file.close()
    f.close()

