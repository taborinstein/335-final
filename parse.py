import os
import random
import string
import subprocess

chars = string.ascii_letters + string.digits

_file = open("Makefile", "r")
makefile = _file.read()
_file.close()

mkname = f"_pmk_{''.join(random.choice(chars) for _ in range(8))}"
pmk = open(mkname, 'a')
current_target = ""
for line in makefile.split("\n"):
    if line.startswith("\t"):
        line = line[1:]
        pmk.write(f'\t@echo -e $@"\\0"$^"\\0" ' + line + "\n")
    else: pmk.write(line + "\n")
pmk.close()

proc = subprocess.run(["make", "-f", mkname], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
os.remove(mkname)
if len(proc.stderr) > 0: 
    print("STDERR", proc.stderr)
    exit(1)

nodeps = []
wdeps = []
objects = []
for line in proc.stdout.decode("utf8").split("\n"):
    if line == "": continue
    (target, deps, commands) = line.split("\0")
    target = target.strip()
    deps = deps.strip()
    if target not in objects: objects.append(target)
    for dep in deps.split(" "): 
        if dep.strip() not in objects and len(dep.strip()) > 0: objects.append(dep.strip())
    commands = commands[1:] # remove leading space
    if commands[0] == '@': commands = commands[1:]
    if len(deps) == 0: nodeps.append(f"@for {target}\n    {commands}")
    else: wdeps.append(f"@for {target} < {deps}\n    {commands}")
_bpmk = open("build.pmk", "w")
_bpmk.write("@objects " + " ".join(objects) + "\n")
_bpmk.write("\n".join(nodeps) + "\n" + "\n".join(wdeps))
_bpmk.close()