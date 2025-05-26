#!/bin/bash
ls main.c && exit 0
seq 1 100 | awk '{print "int _"$0"();"}' > main.c
echo -e "#include <stdio.h>\nint main() {\n" >> main.c
seq 1 100 | awk '{print "    _"$0"();"}' >> main.c
echo -e '    printf("\\n");\n}' >> main.c
seq 1 100 | xargs -I {} sh -c 'echo -e "#include <stdio.h>\nint _{}() {\n    printf(\"{} \");\n    return {};\n}" > _{}.c'