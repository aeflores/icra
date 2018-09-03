import os, git_info, subprocess

fin = open('outputs/__result.out', 'r')
fout = open('result.html', 'w')

fout.write('<!DOCTYPE html>\n')
fout.write('<html>\n')
fout.write('<body>\n')

fout.write('<details>')
fout.write('<summary><font color="blue">[Version Information]</font></summary><br>')
fout.write('WALi-OpenNWA version: '+git_info.getMostRecentCommitHash("."))
fout.write(' ('+git_info.getMostRecentCommitDate(".")+')')
fout.write(' "'+git_info.getMostRecentCommitMessage(".")+'"<br>\n')
duetPath = "../../../../../duet"
fout.write('duet version: '+git_info.getMostRecentCommitHash(duetPath))
fout.write(' ('+git_info.getMostRecentCommitDate(duetPath)+')')
fout.write(' "'+git_info.getMostRecentCommitMessage(duetPath)+'"<br>\n')
fout.write('<pre>'+subprocess.check_output(['opam','list'])+'</pre><br>')
fout.write('</details>\n')

fout.write('<table style="width:100%" border="1">\n')

fout.write('<colgroup>\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:15%;">\n')
fout.write('<col span="1" style="width:10%;">\n')
fout.write('</colgroup>\n')

fout.write('<tr>\n')
fout.write('<th>Test Name</th>\n')
fout.write('<th>Output</th>\n')
fout.write('<th>Duet Output</th>\n')
fout.write('<th>No. of Rounds</th>\n')
fout.write('<th>Result</th>\n')
fout.write('<th>Run Time</th>\n')
fout.write('<th>Duet Result</th>\n')
fout.write('</tr>\n')


firstElem = True
flipResult = False
dirName=os.getcwd()

# these constants are declared in NewtonCompare.cpp
NEWTON_BELOW = 1
NEWTON_ABOVE = 3

bgColor = 0
colCtr = 6

belowTime = 0.0
aboveTime = 0.0
belowAssertionsT = 0
aboveAssertionsT = 0
belowAssertionsF = 0
aboveAssertionsF = 0
totAssertionsT = 0
totAssertionsF = 0

for line in fin:
        if len(line.strip()) == 0 : continue
        words = line.split()
        if (words[0] == "__DIRECTORY"): 
                if firstElem:
                        firstElem = False
                else:
                        fout.write('</tr>\n')
                        fout.write('<tr align=\"center\" style="background-color:#000000;">\n')
                        fout.write('<td><font color=\"#FFFFFF\"><b>Total</b></font></td>\n')
                        fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
                        fout.write('ICRA Assertions (True) = ' + str(belowAssertionsT) + '/' + str(totAssertionsT) + '<br>')
                        #fout.write('Above Assertions (True) = ' + str(aboveAssertionsT) + '/' + str(totAssertionsT) + '</font></td>\n')
                        fout.write('</td>\n')
                        fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
                        fout.write('ICRA Assertions (False) = ' + str(belowAssertionsF) + '/' + str(totAssertionsF) + '<br>')
                        #fout.write('Above Assertions (False) = ' + str(aboveAssertionsF) + '/' + str(totAssertionsF) + '</font></td>\n')
                        fout.write('</td>\n')
                        fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
                        fout.write('ICRA Time = ' + str(belowTime / 1000.0) + '<br>')
                        #fout.write('Above Time = ' + str(aboveTime / 1000.0) + '</font></td>\n')
                        fout.write('</td>\n')
                        fout.write('</tr>\n')
                        belowTime = 0.0
                        aboveTime = 0.0
                        belowAssertionsT = 0
                        aboveAssertionsT = 0
                        belowAssertionsF = 0
                        aboveAssertionsF = 0
                        totAssertionsT = 0
                        totAssertionsF = 0

                fout.write('<tr align=\"center\">\n')
                fout.write('<td colspan=\"7\"><font color=\"#00AAAA\">')
                fout.write(words[1])
                fout.write('</font></td>\n')


        elif (words[0] == "__FILENAME"):
                while (colCtr < 6):
                        colCtr += 1
                        fout.write('<td></td>\n')
                fout.write('</tr>\n')
                
                path=words[1]
                fileName=os.path.basename(path)
                flipResult = (('unsafe' in fileName) or ('false-unreach-call' in fileName))
                
                if (int(words[2]) == NEWTON_ABOVE):
                        runMode = 'above'
                else:
                        runMode = 'below'
                        bgColor = 1 - bgColor
                        if flipResult:
                                totAssertionsF += 1
                        else:
                                totAssertionsT += 1
                
                fout.write('<tr align=\"center\" style="background-color:'+["white","#AAAAAA"][bgColor]+';">\n')
                
                if (runMode == 'below'):
                        fout.write('<td rowspan="1">')
                        fout.write('<a href=\"inputs/' + fileName + '\">' + fileName + '</a>')
                        fout.write('</td>\n')
                
                fout.write('<td>')
                fout.write('<a href=\"outputs/' + fileName + '.' + runMode + '.out\">' + runMode + '</a>')
                fout.write('</td>\n')
                
                if (runMode == 'below'):
                        fout.write('<td rowspan="1">')
                        fout.write('<a href=\"outputs/' + fileName + '.duet.out\">duet</a>')
                        fout.write('</td>\n')
                
                colCtr = 3
        
                
        elif (words[0] == "__NUMRNDS"):
                fout.write('<td>')
                fout.write(words[1])
                fout.write('</td>\n')
                colCtr += 1
        
                
        elif (words[0] == "__NTIME"):
                time = int(words[1]) / 1000000
                if (runMode == 'below'):
                        belowTime += time
                elif (runMode == 'above'):
                        aboveTime += time
                        
                while (colCtr < 5):
                        colCtr += 1
                        fout.write('<td></td>\n')
                fout.write('<td>')
                fout.write(str(time / 1000.0))
                fout.write('</td>\n')
                colCtr += 1
        
        
        elif (words[0] == "__ASSERTION"):
                fout.write('<td>')
                success = True
                if ((len(words) == 1) or flipResult):
                        success = False
                        
                for i in range(len(words)-1):
                        if (flipResult):
                                if (words[i+1] == "FAIL"):
                                        fout.write('<font color=\"#00AA00\">OKAY</font><br>')
                                        success = True 
                                elif (words[i+1] == "PASS"):
                                        fout.write('<b><font color=\"#FF0000\">UNSOUND</font></b><br>')
                                elif (words[i+1] == "__TIMEOUT"):
                                        fout.write('<font color=\"#800080\">TIMEOUT</font><br>')
                                        success = False
                                elif (words[i+1] == "__EXCEPTION"):
                                        fout.write('<font color=\"#800080\">EXCEPTION</font><br>')
                                        success = False
                        else:
                                if (words[i+1] == "PASS"):
                                        fout.write('<font color=\"#00AA00\">PASS</font><br>')
                                elif (words[i+1] == "FAIL"):
                                        fout.write('<font color=\"#FF0000\">FAIL</font><br>')
                                        success = False
                                elif (words[i+1] == "__TIMEOUT"):
                                        fout.write('<font color=\"#800080\">TIMEOUT</font><br>')
                                        success = False
                                elif (words[i+1] == "__EXCEPTION"):
                                        fout.write('<font color=\"#800080\">EXCEPTION</font><br>')
                                        success = False
                
                if (success):
                        if (flipResult and runMode == 'below'):
                                belowAssertionsF += 1
                        elif (not(flipResult) and runMode == 'below'):
                                belowAssertionsT += 1
                        elif (flipResult and runMode == 'above'):
                                aboveAssertionsF += 1
                        elif (not(flipResult) and runMode == 'above'):
                                aboveAssertionsT += 1   
                                
                fout.write('</td>\n')
                colCtr += 1
                
                        
        elif (words[0] == "__DUET"):    
                while (colCtr < 6):
                        colCtr += 1
                        fout.write('<td></td>\n')
                fout.write('<td rowspan="1">')
                for i in range(len(words)-1):
                        if (flipResult):
                                if (words[i+1] == "FAIL"):
                                        fout.write('<font color=\"#00AA00\">OKAY</font><br>')
                                elif (words[i+1] == "PASS"):
                                        fout.write('<b><font color=\"#FF0000\">UNSOUND</font></b><br>')
                                elif (words[i+1] == "TIMEOUT"):
                                        fout.write('<font color=\"#800080\">TIMEOUT</font><br>')
                        else:
                                if (words[i+1] == "PASS"):
                                        fout.write('<font color=\"#00AA00\">PASS</font><br>')
                                elif (words[i+1] == "FAIL"):
                                        fout.write('<font color=\"#FF0000\">FAIL</font><br>')
                                elif (words[i+1] == "TIMEOUT"):
                                        fout.write('<font color=\"#800080\">TIMEOUT</font><br>')
                fout.write('</td>\n')
                colCtr += 1
                
                
        elif (words[0] == "__TIMEOUT"): 
                while (colCtr < 4):
                        colCtr += 1
                        fout.write('<td></td>\n')
                fout.write('<td><font color=\"#800080\">TIMEOUT</font></td>\n')
                colCtr += 1
                
                
        elif (words[0] == "__EXCEPTION"):
                while (colCtr < 4):
                        colCtr += 1
                        fout.write('<td></td>\n')
                fout.write('<td><font color=\"#800080\">EXCEPTION</font></td>\n')
                colCtr += 1
                
fout.write('</tr>\n')
fout.write('<tr align=\"center\" style="background-color:#000000;">\n')
fout.write('<td><font color=\"#FFFFFF\"><b>Total</b></font></td>\n')
fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
fout.write('ICRA Assertions (True) = ' + str(belowAssertionsT) + '/' + str(totAssertionsT) + '<br>')
#fout.write('Above Assertions (True) = ' + str(aboveAssertionsT) + '/' + str(totAssertionsT) + '</font></td>\n')
fout.write('</td>\n')
fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
fout.write('ICRA Assertions (False) = ' + str(belowAssertionsF) + '/' + str(totAssertionsF) + '<br>')
#fout.write('Above Assertions (False) = ' + str(aboveAssertionsF) + '/' + str(totAssertionsF) + '</font></td>\n')
fout.write('</td>\n')
fout.write('<td colspan=\"2\"><font color=\"#FFFFFF\">')
fout.write('ICRA Time = ' + str(belowTime / 1000.0) + '<br>')
#fout.write('Above Time = ' + str(aboveTime / 1000.0) + '</font></td>\n')
fout.write('</td>\n')

fout.write('</tr>\n')
fout.write('</table>\n')

fout.write('</body>\n')
fout.write('</html>\n')

fout.close()

