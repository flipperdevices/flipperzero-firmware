/*
TEST: pipe record

1. create pipe record
2. Open/subscribe to it 
3. write data
4. check that subscriber get data
5. try to read, get error
6. close record
7. try to write, get error
*/



/*
TEST: holding data

1. Create holding record
2. Open/Subscribe on it
3. Write data
4. Read and check data, that subscriber get data
5. Try to write/read wrong size of data
6. Create another task, do concurrent access to data with take/give
*/

/*
TEST: non-existent data
1. Try to open non-existent record
2. Check for NULL handler
3. Try to write/read, get error
*/

/*
TEST: mute algorithm
1. Create "parent" application:
    1. Create pipe record
    2. Open watch handler: no_mute=false, solo=false, subscribe to data and state.

2. Open handler A: no_mute=false, solo=false, NULL subscriber. Subscribe to state.
Try to write data to A and check subscriber.

3. Open handler B: no_mute=true, solo=true, NULL subscriber.
Check A state cb get FlipperRecordStateMute.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data.

4. Open hadler C: no_mute=false, solo=true, NULL subscriber.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data. (not muted because open with no_mute)
Try to write data to C and check that subscriber get data.

5. Open handler D: no_mute=false, solo=false, NULL subscriber.
Try to write data to A and check that subscriber get no data. (muted)
Try to write data to B and check that subscriber get data. (not muted because open with no_mute)
Try to write data to C and check that subscriber get data. (not muted because D open without solo)
Try to write data to D and check that subscriber get data.

8. Close C, close B.
Check A state cb get FlipperRecordStateUnmute
Try to write data to A and check that subscriber get data. (unmuted)
Try to write data to D and check that subscriber get data.

9. Exit "parent application"
Check A state cb get FlipperRecordStateDeleted
*/