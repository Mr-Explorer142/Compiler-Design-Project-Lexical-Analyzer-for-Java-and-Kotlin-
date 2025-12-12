// ==========================================================
// 150+ LINE TEST FILE FOR LEXICAL ANALYZER
// Contains: Keywords, identifiers, operators, errors, comments
// ==========================================================

/*
   Multi-line comment block
   This file will be used to test:
   - Tokenization
   - Comment logging
   - Misspelled keywords
   - Type mismatch errors
   - Misplaced relational operators
   - Identifier declaration errors
*/

public class Input2 {

    // ------------------- DECLARATIONS ---------------------

    int x;
    float y;
    char c;

    // Misspelled keywords (E2)
    inti wrong1 = 5;
    flaot wrong2 = 10;
    dobule wrong3 = 20;

    // Proper declarations
    int num = 5;
    float temp = 21.9;
    char letter = 'Z';

    // Type mismatches (E1)
    int badInt1 = 3.14;
    float badFloat1 = 'c';
    char badChar1 = "wrong";

    // Identifier used before declaration (E3)
    preDecl=99;

    // Now correct declaration
    int preDecl;

    // Misspelled identifiers (E3)
    temprature=30;numbr=22;lettter='B';

    // ======================================================
    // Misplaced relational operators (E4) – guaranteed triggers
    // ======================================================

    < // operator alone on a line
    > // operator alone on a line
    <= // operator alone on a line
    >= // operator alone on a line
    == // operator alone on a line
    != // operator alone on a line

    x< // missing right operand
    <
    y                 // missing left operand

    x<= // missing right operand
    >=y // missing left operand

    ==
    y // missing left operand
    x!= // missing right operand

    x <> y // invalid relational operator pattern
    x < > y // separated operator pieces

    x y>= // original misplaced operator (kept)

    // ======================================================
    // Correct relational operator usage
    // ======================================================

    if(x<y)
    {
        x = x + 1;
    }

    // Many comments to test comment logging
    // Comment 1
    // Comment 2
    // Comment 3

    /*
     * Long comment block
     * Should appear in comment list
     */

    // ------------------- BEGIN STRESS SECTION ---------------------

    int a1 = 1;
    int a2 = 2;
    int a3 = 3;
    int a4 = 4;
    int a5 = 5;

    float f1 = 1.1;
    float f2 = 2.2;
    float f3 = 3.3;

    char c1 = 'A';
    char c2 = 'B';
    char c3 = 'C';

    // Repeated operations (to generate many tokens)
    a1=a1+1;a2=a2+2;a3=a3+3;a4=a4+4;a5=a5+5;

    for(
    int i = 0;i<10;i++)
    {
        x = x + i;
        y = y + 0.5;
    }

    // ------------------- FILL LINES: SIMPLE CODE BLOCKS ---------------------

    int sum = 0;for(
    int i = 0;i<20;i++)
    {
        sum = sum + i;
    }

    // Dozens of repeated valid lines to push above 150 lines
    int t1 = 1;
    int t2 = 2;
    int t3 = 3;
    int t4 = 4;
    int t5 = 5;
    int t6 = 6;
    int t7 = 7;
    int t8 = 8;
    int t9 = 9;
    int t10 = 10;

    t1=t1+1;t2=t2+2;t3=t3+3;t4=t4+4;t5=t5+5;t6=t6+6;t7=t7+7;t8=t8+8;t9=t9+9;t10=t10+10;

    // Additional repeated blocks
    int block1 = 100;
    int block2 = 200;
    int block3 = 300;
    int block4 = 400;
    int block5 = 500;
    int block6 = 600;
    int block7 = 700;
    int block8 = 800;
    int block9 = 900;
    int block10 = 1000;

    block1=block1-10;block2=block2-20;block3=block3-30;block4=block4-40;block5=block5-50;block6=block6-60;block7=block7-70;block8=block8-80;block9=block9-90;block10=block10-100;

    // Even more filler lines
    int j1 = 11;
    int j2 = 22;
    int j3 = 33;
    int j4 = 44;
    int j5 = 55;
    int j6 = 66;
    int j7 = 77;
    int j8 = 88;
    int j9 = 99;
    int j10 = 111;

    j1++;j2++;j3++;j4++;j5++;j6++;j7++;j8++;j9++;j10++;

    // 20+ more lines to exceed 150 easily
    int z1 = 1;
    int z2 = 2;
    int z3 = 3;
    int z4 = 4;
    int z5 = 5;
    int z6 = 6;
    int z7 = 7;
    int z8 = 8;
    int z9 = 9;
    int z10 = 10;
    int z11 = 11;
    int z12 = 12;
    int z13 = 13;
    int z14 = 14;
    int z15 = 15;
    int z16 = 16;
    int z17 = 17;
    int z18 = 18;
    int z19 = 19;
    int z20 = 20;

    z1=z1*2;z2=z2*2;z3=z3*2;z4=z4*2;z5=z5*2;z6=z6*2;z7=z7*2;z8=z8*2;z9=z9*2;z10=z10*2;z11=z11*2;z12=z12*2;z13=z13*2;z14=z14*2;z15=z15*2;z16=z16*2;z17=z17*2;z18=z18*2;z19=z19*2;z20=z20*2;

    // End message
    // End message
    // End message
    // End message

} // End of class
