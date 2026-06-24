# iButton — Test Cases

## 1. iButton Dallas

Steps:
1. Read keys Dallas types 1971, 1990, 1992, 1996 and save
   **Expected result**
     1. Keys read and saved

2. Go to iButton -> Add Manually
3. Create keys Dallas types 1971, 1990, 1992, 1996
   **Expected result**
     1. Keys were created and saved with the specified name and data

4. Emulate Dallas keys and consider it as different devices
   **Expected result**
     1. The keys were successfully read, the data is identical

5. Write the saved keys to the RW2004 blank, and read the key with the flipper
   **Expected result**
     1. The keys were successfully written, the data on each type of key corresponds to the created ones

6. Edit Keys
   **Expected result**
     1. Keys edited

7. Delete keys
   **Expected result**
     1. Keys Deleted

## 2. Checking cards and protocols. Dallas DS1992

Steps:
1. Go to iButton
2. Shared step [1]: Read
   1. Read the subject using the flipper
     **Expected result**
           1. The subject read successfully
           2. The information on the read key corresponds to the original

3. Shared step [2]: Emulation
   1. Emulate the read key and read it as another flipper
     **Expected result**
           1. The key was considered another flipper
           2. The information on the key is identical to the original.

4. Shared step [3]: Save
   1. Save previously read key
     **Expected result**
       1. The key was saved with the specified name.
       2. After saving, the scene changed to the "Saved" directory.

5. Shared step [4]: Write
   1. Write a previously saved key to another storage device using the "write" function
     **Expected result**
       1. The card was recorded, a success scene appeared

6. Shared step [5]: Read the written
   1. Read the media on which the key was previously written
     **Expected result**
       1. The information from the carrier was considered
       2. The data is identical to the stored key that was written

## 3. Checking cards and protocols. Cyfral

Steps:
1. Go to iButton
2. Shared step [1]: Read
   1. Read the subject using the flipper
     **Expected result**
           1. The subject read successfully
           2. The information on the read key corresponds to the original

3. Shared step [2]: Emulation
   1. Emulate the read key and read it as another flipper
     **Expected result**
           1. The key was considered another flipper
           2. The information on the key is identical to the original.

4. Shared step [3]: Save
   1. Save previously read key
     **Expected result**
           1. The key was saved with the specified name.
           2. After saving, the scene changed to the "Saved" directory.

## 4. Checking cards and protocols. Dallas DS1996

Steps:
1. Go to iButton
2. Shared step [1]: Read
   1. Read the subject using the flipper
     **Expected result**
       1. The subject read successfully
       2. The information on the read key corresponds to the original

3. Shared step [2]: Emulation
   1. Emulate the read key and read it as another flipper
     **Expected result**
       1. The key was considered another flipper
       2. The information on the key is identical to the original.

4. Shared step [3]: Save
   1. Save previously read key
     **Expected result**
       1. The key was saved with the specified name.
       2. After saving, the scene changed to the "Saved" directory.

5. Shared step [4]: Write
   1. Write a previously saved key to another storage device using the "write" function
     **Expected result**
       1. The card was recorded, a success scene appeared

6. Shared step [5]: Read the written
   1. Read the media on which the key was previously written
     **Expected result**
       1. The information from the carrier was considered
       2. The data is identical to the stored key that was written

## 5. iButton Cyfral

Steps:
1. Read Cyfral key
   **Expected result**
     1. Key read and saved

2. Open "Add manually" menu
3. Create key Cyfral
   **Expected result**
     1. Key was created and saved with the specified name and data

4. Emulate Cyfral key and consider it as different devices
   **Expected result**
     1. The keys were successfully read, the data is identical

5. Edit key
   **Expected result**
     1. Key edited

6. Delete key
   **Expected result**
     1. Key Deleted

## 6. iButton Reading

Steps:
1. Go to iButton -> Read
2. Read the key
   **Expected result**
     1. Reading, working with the key

3. Save the key
   **Expected result**
     1. Key saved

## 7. iButton Metakom

Steps:
1. Read Metakom key
   **Expected result**
     1. Key read and saved

2. Open "Add manually" menu
3. Create key Metakom
   **Expected result**
     1. Key was created and saved with the specified name and data

4. Emulate Metakom key and consider it as different devices
   **Expected result**
     1. The keys were successfully read, the data is identical

5. Edit key
   **Expected result**
     1. Key edited

6. Delete key
   **Expected result**
     1. Key Deleted

## 8. Checking cards and protocols. Dallas DS1990

Steps:
1. Go to iButton
2. Shared step [1]: Read
   1. Read the subject using the flipper
     **Expected result**
       1. The subject read successfully
       2. The information on the read key corresponds to the original

3. Shared step [2]: Emulation
   1. Emulate the read key and read it as another flipper
     **Expected result**
       1. The key was considered another flipper
       2. The information on the key is identical to the original.

4. Shared step [3]: Save
   1. Save previously read key
     **Expected result**
       1. The key was saved with the specified name.
       2. After saving, the scene changed to the "Saved" directory.

5. Shared step [4]: Write
   1. Write a previously saved key to another storage device using the "write" function
     **Expected result**
       1. The card was recorded, a success scene appeared

6. Shared step [5]: Read the written
   1. Read the media on which the key was previously written
     **Expected result**
       1. The information from the carrier was considered
       2. The data is identical to the stored key that was written

## 9. Checking cards and protocols. Dallas DS1971

Steps:
1. Go to iButton
2. Shared step [1]: Read
   1. Read the subject using the flipper
     **Expected result**
       1. The subject read successfully
       2. The information on the read key corresponds to the original

3. Shared step [2]: Emulation
   1. Emulate the read key and read it as another flipper
     **Expected result**
       1. The key was considered another flipper
       2. The information on the key is identical to the original.

4. Shared step [3]: Save
   1. Save previously read key
     **Expected result**
       1. The key was saved with the specified name.
       2. After saving, the scene changed to the "Saved" directory.

5. Shared step [4]: Write
   1. Write a previously saved key to another storage device using the "write" function
     **Expected result**
       1. The card was recorded, a success scene appeared

6. Shared step [5]: Read the written
   1. Read the media on which the key was previously written
     **Expected result**
       1. The information from the carrier was considered
       2. The data is identical to the stored key that was written
