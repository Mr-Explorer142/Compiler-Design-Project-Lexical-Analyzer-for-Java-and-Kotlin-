// ==========================================================
// Input2.kt - 150+ line Kotlin test file for lexical analyzer
// Contains: package/import, comments, var/val declarations,
//           type mismatches (E1), misspelled keywords (E2),
//           undeclared identifiers (E3), misplaced relational operators (E4)
// ==========================================================

/*
 Multi-line comment block
 This file will test Kotlin-specific declarations and errors
*/

package com.example.test.project

import kotlin.text.StringBuilder
import kotlin.collections.List

// Simple function
fun main() {
    // ---------------- Declarations ----------------
    var x: Int
    val y: Float = 2.5f
    var ch: Char = 'A'
    val name: String = "KotlinTest"

    // ---------------- Valid assignments ----------------
    x = 10
    var z: Int = 20
    z = z + 5
    var f: Float = 3.14f
    val s: String = "hello"

    // ---------------- Misspelled keyword (E2) ----------------
    vaar badVar = 5         // should be 'var' -> E2

    // ---------------- Type mismatches (E1) ----------------
    var a: Int = 3.14          // E1: Int cannot take float literal
    var b: Float = 'c'         // E1: Float cannot take char literal
    var c: Char = "hello"      // E1: Char cannot take string literal

    // ---------------- Undeclared identifier usage (E3) ----------------
    undeclaredVar = 10         // E3: used before declaration

    // now declare it correctly
    var undeclaredVar: Int

    // ---------------- Misspelled identifier (E3) ----------------
    valuee = 100               // E3
    val value: Int = 20        // correct declaration

    // ---------------- Misplaced relational operators (E4) ----------------
    <
    >
    <=
    >=
    ==
    !=

    x <
    < y

    x <=
    >= y

    == y
    x !=

    // some valid relational usages
    if (x < z) {
        x = x + 1
    }

    // ---------------- Kotlin-specific patterns ----------------
    var maybe: String? = null
    var len = maybe?.length ?: 0

    // ---------------- Filler to extend file length ----------------
    var a1: Int = 1
    var a2: Int = 2
    var a3: Int = 3
    var a4: Int = 4
    var a5: Int = 5
    var a6: Int = 6
    var a7: Int = 7
    var a8: Int = 8
    var a9: Int = 9
    var a10: Int = 10

    a1 = a1 + 1; a2 = a2 + 2; a3 = a3 + 3; a4 = a4 + 4; a5 = a5 + 5;
    a6 = a6 + 6; a7 = a7 + 7; a8 = a8 + 8; a9 = a9 + 9; a10 = a10 + 10;

    // many repetitive lines to push above 150 lines
    var t1: Int = 1
    var t2: Int = 2
    var t3: Int = 3
    var t4: Int = 4
    var t5: Int = 5
    var t6: Int = 6
    var t7: Int = 7
    var t8: Int = 8
    var t9: Int = 9
    var t10: Int = 10

    t1 = t1 + 1
    t2 = t2 + 2
    t3 = t3 + 3
    t4 = t4 + 4
    t5 = t5 + 5
    t6 = t6 + 6
    t7 = t7 + 7
    t8 = t8 + 8
    t9 = t9 + 9
    t10 = t10 + 10

    var block1: Int = 100
    var block2: Int = 200
    var block3: Int = 300
    var block4: Int = 400
    var block5: Int = 500
    var block6: Int = 600
    var block7: Int = 700
    var block8: Int = 800
    var block9: Int = 900
    var block10: Int = 1000

    block1 = block1 - 10
    block2 = block2 - 20
    block3 = block3 - 30
    block4 = block4 - 40
    block5 = block5 - 50
    block6 = block6 - 60
    block7 = block7 - 70
    block8 = block8 - 80
    block9 = block9 - 90
    block10 = block10 - 100

    // more filler loop
    for (i in 0..19) {
        sumIteration(i)
    }

    // define sumIteration to avoid extra errors
    fun sumIteration(n: Int) {
        var localSum: Int = 0
        for (j in 0..n) localSum = localSum + j
    }

    // more lines to ensure >150
    var z1: Int = 1
    var z2: Int = 2
    var z3: Int = 3
    var z4: Int = 4
    var z5: Int = 5
    var z6: Int = 6
    var z7: Int = 7
    var z8: Int = 8
    var z9: Int = 9
    var z10: Int = 10
    var z11: Int = 11
    var z12: Int = 12
    var z13: Int = 13
    var z14: Int = 14
    var z15: Int = 15
    var z16: Int = 16
    var z17: Int = 17
    var z18: Int = 18
    var z19: Int = 19
    var z20: Int = 20

    z1 = z1 * 2; z2 = z2 * 2; z3 = z3 * 2; z4 = z4 * 2; z5 = z5 * 2;
    z6 = z6 * 2; z7 = z7 * 2; z8 = z8 * 2; z9 = z9 * 2; z10 = z10 * 2;
    z11 = z11 * 2; z12 = z12 * 2; z13 = z13 * 2; z14 = z14 * 2; z15 = z15 * 2;
    z16 = z16 * 2; z17 = z17 * 2; z18 = z18 * 2; z19 = z19 * 2; z20 = z20 * 2;

    // end of main
} // end fun main

// Small helper function outside main (to increase file length)
fun helper() {
    var h1: Int = 1
    var h2: Int = 2
    var h3: Int = 3
    h1 = h1 + h2 + h3
}

// End of file - many lines present above (>150 lines)
