*
       PROGRAM BLINK
       DOUBLE PRECISION A(2,2), B(2,2), C(2,2)
       EXTERNAL DGEMM
       CALL DGEMM('n', 'n', 2, 2, 2, 1.0D0, A, 2, B, 2, 1.0D0, C, LDC)
       STOP
       END
