def filter(x):
    f = 0
    f |= (0xf22c0 >> (x & 0xf) & 16)
    f |= (0x6c9c0 >> (x >> 4 & 0xf) & 8)
    f |= (0x3c8b0 >> (x >> 8 & 0xf) & 4)
    f |= (0x1e458 >> (x >> 12 & 0xf) & 2)
    f |= (0x0d938 >> (x >> 16 & 0xf) & 1)
    return (0xEC57E80A >> f) & 1

# C code
"""
    for(data[ tbl ] <<= 1; tbl <= end; data[++tbl] <<= 1) {
        if ((filter(data[ tbl ]) ^ filter(data[ tbl ] | 1)) !=0 )
            data[ tbl ] |= filter(data[ tbl ]) ^ bit;
        else if (filter(data[ tbl ]) == bit) {
            data[ ++end ] = data[ ++tbl ];
            data[ tbl ] = data[ tbl - 1 ] | 1;
        } else {
            data[ tbl-- ] = data[ end-- ];
        }
    }
"""

# Python code
"""
def extend_table_simple(data, tbl, end, bit):
    data[tbl] <<= 1
    while tbl <= end:
        if (filter(data[tbl]) ^ filter(data[tbl] | 1)) != 0:
            data[tbl] |= filter(data[tbl]) ^ bit
        elif filter(data[tbl]) == bit:
            end += 1
            tbl += 1
            data[end] = data[tbl]
            data[tbl] = data[tbl - 1] | 1
        else:
            data[tbl] = data[end]
            end -= 1
            tbl -= 1
        tbl += 1
        data[tbl] <<= 1
    return end
"""

# Python code that's a bit more readable
def extend_table_simple(data, tbl, end, bit):
    data[tbl] <<= 1
    while tbl <= end:
        x = data[tbl]
        if (filter(x) ^ filter(x | 1)) != 0:
            data[tbl] |= filter(x) ^ bit
        elif filter(x) == bit:
            end += 1
            tbl += 1
            data[end] = data[tbl]
            data[tbl] = data[tbl - 1] | 1
        else:
            data[tbl] = data[end]
            end -= 1
            tbl -= 1
        tbl += 1
        data[tbl] <<= 1
    return end

data_array=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
# result: [2, 3, 5, 9, 41, 12, 13, 14, 15, 21, 18, 19, 25, 28, 29, 30, 31, 37, 34, 35, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
new_end=extend_table_simple(data_array, 0, 19, 1)
a=set(data_array)
print(data_array)
data_array=[1,2,3,4,5,6,7,8,9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
# result: [2, 3, 5, 9, 21, 12, 13, 14, 15, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
new_end=extend_table_simple(data_array, 0, 9, 1)
b=set(data_array)
print(data_array)
data_array=[11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
new_end=extend_table_simple(data_array, 0, 9, 1)
c=set(data_array)
print(data_array)
print(a)
print(set(list(b)+list(c)))
