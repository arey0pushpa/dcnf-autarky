#!/usr/bin/env python3

with open('input0_1_2qbf.cnf', 'r') as myfile:
    lines = myfile.readlines()
    newlines = []
    universals = set()
    
    seen_header = False
    
    for line in lines:
        # make UV and EV comments and remember universals
        if line.startswith('UV'):
            words = line.split()
            universals.add(int(words[1])) # remember universals
            newlines.append("c " + line)
        elif line.startswith('EV'):
            newlines.append("c " + line)
        elif line.startswith('p cnf '):
            # parse header
            words = line.split()
            var_num = int(words[2])
            newlines.append(line)
            
            universals_line = 'a ' + ' '.join(str(x) for x in sorted(list(universals))) + ' 0\n'
            newlines.append(universals_line)
            
            existentials_line = 'e ' + ' '.join(str(x) for x in range(1,var_num+1) if not (x in universals)) + ' 0\n'
            newlines.append(existentials_line)
            
            seen_header = True
            
        else:
            if not (seen_header and line.startswith('c ')):
                newlines.append(line)
            
    if len(universals) > 0 and seen_header:
        print(''.join(newlines))
    