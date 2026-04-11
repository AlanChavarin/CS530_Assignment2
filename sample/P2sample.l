    1                                      
    2   0000 SUM       START   0           
    3   0000 FIRST     LDX     #0          050000
    4   0003           LDA     #0          010000
    5   0006           +LDB    #TABLE2     69101794
    6   000A           BASE    TABLE2      
    7   000A LOOP      ADD     TABLE,X     1BA017
    8   000D           ADD     TABLE2,X    1BC000
    9   0010           TIX     COUNT       2F200E
   10   0013           JLT     LOOP        3B2FF4
   11   0016           +STA    TOTAL       0F102F04
   12   001A           RSUB                4F0000
   13   001D MYLIT     +LDA    =C'EOF'     03102F07
   14   0021 COUNT     RESW    1           
   15   0024 TABLE     RESW    2000        
   16   1794 TABLE2    RESW    2000        
   17   2F04 TOTAL     RESW    1           
   18   2F07 *         =C'EOF'             454F46
   19   2F0A           END     FIRST       
