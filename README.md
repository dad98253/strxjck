# strxjck
a program to find certain string literals in c code, build a string table, and replace each literal with a string table reference

 usage:
 \dos\find \users\dad\passwin -name *.cpp -print0 -maxdepth 1 | xargs -0 strxjck

 or, on WINDOZ (one file at a time):

 dir /s /b *.cpp > temp.txt
 FOR /F %%k in (temp.txt) DO strxjck %%k

The program assumes that the file name passed as a argument is a c++ program. It searches the program for calls to the passwin MsgBox function. If an argument to MsgBox is a string literal, it is replaced by an integer constant. The string is added to a string table (file johns.txt) and an apprpriate #define (for the constant) is added to the johnsstr.h.txt file. The input file is written unchanged (but for the string subsitutions) to a file of the same name with "_strx" added to the base file name. (e.g., "strxjck passwordsW.cpp" will produce output files "johns.txt", "johnsstr.h.txt", and "passwordsW_strx.cpp"). Therefore, if one wished to remove all string literals to MsgBox calls, one would need to incorporate johnsstr.h into the appropriate include file (probably the one that defines the MsgBox calling sequence), add the string table data in johns.txt into your resource file, and modify the source to MsgBox to look up the string table references...

Notes:
1) strxjck expects to be executred in the same folder that the input file is located in
