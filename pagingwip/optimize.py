# Modify extend_table_new() to produce the same result as extend_table_original()
# extend_table_new() uses smaller slices of the original list

def filter(x):
    f = 0
    f |= (0xf22c0 >> (x & 0xf) & 16)
    f |= (0x6c9c0 >> (x >> 4 & 0xf) & 8)
    f |= (0x3c8b0 >> (x >> 8 & 0xf) & 4)
    f |= (0x1e458 >> (x >> 12 & 0xf) & 2)
    f |= (0x0d938 >> (x >> 16 & 0xf) & 1)
    return (0xEC57E80A >> f) & 1

def extend_table_original(data, start, end, bit):
    data[start] <<= 1
    while start <= end:
        x = data[start]
        if (filter(x) ^ filter(x | 1)) != 0:
            data[start] |= filter(x) ^ bit
        elif filter(x) == bit:
            end += 1
            start += 1
            data[end] = data[start]
            data[start] = data[start - 1] | 1
        start += 1
        data[start] <<= 1
    return end

"""
def extend_table_new(data, start, end, bit):
    while start <= end:
        data[start] <<= 1
        x = data[start]
        if (filter(x) ^ filter(x | 1)) != 0:
            data[start] |= filter(x) ^ bit
        elif filter(x) == bit:
            data.insert(start+1, data[start] | 1)
            end += 1
            start += 1
        start += 1
    return end
"""

def extend_table_new(data, start, end, bit):
    new = []
    for i in range(start, end):
        data[i] <<= 1
        x = data[i]
        if (filter(x) ^ filter(x | 1)) != 0:
            data[i] |= filter(x) ^ bit
        elif filter(x) == bit:
            new.append(data[i] | 1)
    data += new
    return end

# Unit testing code below
simulated_size=100

print("Original, 1/4:")
data_array_orig=list(range(0,simulated_size))+([0]*(simulated_size*5))
new_end_orig=extend_table_original(data_array_orig, 0, simulated_size, 1)
a=set(data_array_orig)
print(sorted(a))

print("New, 1/4:")
data_array_new=list(range(0,simulated_size)) #+([0]*(simulated_size*5))
new_end_new=extend_table_new(data_array_new, 0, simulated_size, 1)
b=set(data_array_new)
print(sorted(b))

"""
print("Test result:")
bits = [0, 0, 1]
#for x in range(0, 3):
for x in range(0, 1):
    bit = bits[x]
    new_end_orig=extend_table_original(data_array_orig, 0, new_end_orig, bit)
    #chunks = [data_array_new[x:x+20] for x in range(0, len(data_array_new), 20)]
    #for chunk in chunks:
    #    new_end_new=extend_table_new(chunk, 0, new_end_new, bit)
    #data_array_new = sum(chunks, [])
    new_end_new=extend_table_new(data_array_new, 0, new_end_new, bit)
if set(data_array_orig)==set(data_array_new):
    print("\033[32mPass\033[0m")
else:
    print("\033[31mFail\033[0m")
"""
