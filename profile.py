funcs = []
scripts = []

with open("profile.log", "r") as file:
    scriptname = ""
    functionname = ""
    totaltime = 0
    for line in file:
        if line[:6] == "SCRIPT":
            scriptname = line.strip()
        elif line[:6] == "FUNCTI":
            functionname = line.strip()
        elif line[:10] == "Total time":
            totaltime = float(line[11:])
            if scriptname != "":
                scripts.append([scriptname, totaltime])
                scriptname = ""
            elif functionname != "":
                funcs.append([functionname, totaltime])
                functionname = ""
        

# Sort the script times
scripts.sort(key = lambda x: x[1], reverse = True)
funcs.sort(key = lambda x: x[1], reverse = True)

for script in scripts:
    print("{} {}".format(script[0], script[1]))

print()

for func in funcs[:50]:
    print("{} {}".format(func[0], func[1]))
