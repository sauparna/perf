## Introduction

_everybit_ is an implementation of a bit array (bit vector). Given a
naive implementation at the outset, we improve it to a final form and
compare performance.

The function bitarray_rotate() rotates a specified subset of the bit
array a given number of times. The project's goal is to improve the
performance of the rotation.

### Performance Tests

The test runs many rotations calling bitarray_rotate() and times the
calls. It progressively increases the size of the bit array, and the
number of shifts, and quits when the duration of rotation exceeds a
specified time limit. The performance test code outputs this
information as a table shown below. The first column numbers each
iteration and the corresponding size-shift-time information is in
subsequent columns. As we can see, the implementation at the outset
can drive the test upto tier 17 before exceeding the 1 second limit,
and then our final version reports tier 33 under the second.

#### Outset

    TIER SIZE(B)         #SHIFTS         TIME(s)   
    0    0               2               0.000006
    1    0               3               0.000004
    2    1               5               0.000005
    3    1               8               0.000008
    4    2               13              0.000016
    5    4               21              0.000035
    6    6               34              0.000065
    7    11              55              0.000166
    8    18              89              0.000264
    9    29              144             0.000372
    10   47              233             0.001001
    11   76              377             0.002742
    12   123             610             0.006815
    13   199             987             0.016147
    14   323             1597            0.043135
    15   522             2584            0.115130
    16   845             4181            0.301183
    17   1368            6765            0.791499
    18   2213            10946           2.066225 exceeded 1.00s cutoff

#### Final

    TIER SIZE(B)         #SHIFTS         TIME(s)   
    0    0               2               0.000006
    1    0               3               0.000001
    2    1               5               0.000001
    3    1               8               0.000001
    4    2               13              0.000001
    5    4               21              0.000002
    6    6               34              0.000003
    7    11              55              0.000005
    8    18              89              0.000007
    9    29              144             0.000011
    10   47              233             0.000017
    11   76              377             0.000027
    12   123             610             0.000042
    13   199             987             0.000067
    14   323             1597            0.000108
    15   522             2584            0.000174
    16   845             4181            0.000280
    17   1368            6765            0.000454
    18   2213            10946           0.000733
    19   3582            17711           0.001198
    20   5796            28657           0.001922
    21   9378            46368           0.002979
    22   15174           75025           0.004459
    23   24552           121393          0.006780
    24   39726           196418          0.010974
    25   64278           317811          0.017813
    26   104005          514229          0.028610
    27   168283          832040          0.046151
    28   272288          1346269         0.074530
    29   440572          2178309         0.121253
    30   712860          3524578         0.195876
    31   1153433         5702887         0.317154
    32   1866294         9227465         0.509628
    33   3019727         14930352        0.822929
    34   4886021         24157817        1.351409 exceeded 1.00s cutoff

## Optimizations

* In contrast to the naive rotation, where bits in the given range is
  traversed linearly copying the adjacent bit to the current position
  and finally placing the edge-bit at the end, we use the following
  identity that performs n shifts (left shifts, say) using reverse
  operations (denoted <sup>R</sup> ):

  (a<sup>R</sup>b<sup>R</sup>)<sup>R</sup> = ba

  where a is the n-element prefix that is shifted off the edge and
introduced from the right.

* We avoid any function calls and perform the three reverse operations
  by indexing into the bit array buffer, avoiding use of the get and
  set methods. We also avoid calling a reverse() function thrice and
  instead code in three identical reverse loops.
