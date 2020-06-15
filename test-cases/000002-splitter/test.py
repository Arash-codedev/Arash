import sys
import subprocess
import os

bin_dir = '../../bin/'
source_filename = 'crc_calculator.ara'
streamer_binary = './streamer'
separator_binary = './separator'
output_filename = 'separator-out-piped.txt'
file_content = '''from std import print, String, hex_str, endl

type CRC_Number = UInt8


class Hash:
    var:
        name : String
        init : CRC_Number
        final_xor : CRC_Number
        polynomial : CRC_Number
        ref_in : Bool
        ref_out : Bool


fn main:
    -> int
body:
    var msg = '123456789' : String

    var crc32 : Hash
    crc32.name = 'CRC-32'
    crc32.init = 0xFFFFFFFF
    crc32.final_xor = 0xFFFFFFFF
    crc32.polynomial = 0x04C11DB7
    crc32.ref_in = true
    crc32.ref_out = true

    var result = crc_hash(msg, crc32) : CRC_Number
    print('Message: "', msg, '"', endl)
    print('CRC_Number-32: ', hex_str(result), '', endl)

    return 0


fn reflect:
    x : UInt32
    N : const UInt8
    -> UInt32
body:
    var y = 0 : UInt32

    for var i = 0 : UInt8; i < N; i++ :
        if x & 0x01 :
            y |= (1 << ((N - 1) - i))
        x = (x >> 1)

    return y


fn crc_hash:
    message : const String ref
    hash : const Hash ref
    -> CRC_Number
body:
    var remainder = hash.init : CRC_Number

    const width = (8 * CRC_Number.byte_size) : int
    const top_bit = (1 << (width - 1)) : int

    for var i = 0 : int; i < message.length(); i++ :

        var data_in : UInt8

        if hash.ref_in :
            data_in = reflect((UInt8) message[i], 8)
        else
            data_in = (UInt8) message[i]

        remainder ^= (data_in << (width - 8))

        for var bit = 8 : UInt8; bit > 0; bit-- :
            if remainder & top_bit :
                remainder = (remainder << 1) ^ hash.polynomial
            else
                remainder = (remainder << 1)

    if hash.ref_out :
        remainder = (CRC_Number) reflect(remainder, width)

    return (remainder ^ hash.final_xor)
'''

expected_result = '''Module: crc_calculator.ara
from| |std| |import| |print|,| |String|,| |hex_str|,| |endl

type| |CRC_Number| |=| |UInt8


class| |Hash|:
    |var|:
        |name| |:| |String
        |init| |:| |CRC_Number
        |final_xor| |:| |CRC_Number
        |polynomial| |:| |CRC_Number
        |ref_in| |:| |Bool
        |ref_out| |:| |Bool


fn| |main|:
    |->| |int
body|:
    |var| |msg| |=| |'|123456789|'| |:| |String

    |var| |crc32| |:| |Hash
    |crc32|.|name| |=| |'|CRC|-|32|'
    |crc32|.|init| |=| |0xFFFFFFFF
    |crc32|.|final_xor| |=| |0xFFFFFFFF
    |crc32|.|polynomial| |=| |0x04C11DB7
    |crc32|.|ref_in| |=| |true
    |crc32|.|ref_out| |=| |true

    |var| |result| |=| |crc_hash|(|msg|,| |crc32|)| |:| |CRC_Number
    |print|(|'|Message|:| |"|'|,| |msg|,| |'|"|'|,| |endl|)
    |print|(|'|CRC_Number|-|32|:| |'|,| |hex_str|(|result|)|,| |'|'|,| |endl|)

    |return| |0


fn| |reflect|:
    |x| |:| |UInt32
    |N| |:| |const| |UInt8
    |->| |UInt32
body|:
    |var| |y| |=| |0| |:| |UInt32

    |for| |var| |i| |=| |0| |:| |UInt8|;| |i| |<| |N|;| |i|++| |:
        |if| |x| |&| |0x01| |:
            |y| |||=| |(|1| |<<| |(|(|N| |-| |1|)| |-| |i|)|)
        |x| |=| |(|x| |>>| |1|)

    |return| |y


fn| |crc_hash|:
    |message| |:| |const| |String| |ref
    |hash| |:| |const| |Hash| |ref
    |->| |CRC_Number
body|:
    |var| |remainder| |=| |hash|.|init| |:| |CRC_Number

    |const| |width| |=| |(|8| |*| |CRC_Number|.|byte_size|)| |:| |int
    |const| |top_bit| |=| |(|1| |<<| |(|width| |-| |1|)|)| |:| |int

    |for| |var| |i| |=| |0| |:| |int|;| |i| |<| |message|.|length|(|)|;| |i|++| |:

        |var| |data_in| |:| |UInt8

        |if| |hash|.|ref_in| |:
            |data_in| |=| |reflect|(|(|UInt8|)| |message|[|i|]|,| |8|)
        |else
            |data_in| |=| |(|UInt8|)| |message|[|i|]

        |remainder| |^|=| |(|data_in| |<<| |(|width| |-| |8|)|)

        |for| |var| |bit| |=| |8| |:| |UInt8|;| |bit| |>| |0|;| |bit|--| |:
            |if| |remainder| |&| |top_bit| |:
                |remainder| |=| |(|remainder| |<<| |1|)| |^| |hash|.|polynomial
            |else
                |remainder| |=| |(|remainder| |<<| |1|)

    |if| |hash|.|ref_out| |:
        |remainder| |=| |(|CRC_Number|)| |reflect|(|remainder|,| |width|)

    |return| |(|remainder| |^| |hash|.|final_xor|)
'''

os.chdir(bin_dir)

# action

# print('Testing the splitter ...')

with open(source_filename, "w") as source_file_obj:
    source_file_obj.write(file_content)


subprocess.call([streamer_binary, source_filename])
subprocess.call([separator_binary])

with open(output_filename, 'r') as output_file_obj:
    output_result = output_file_obj.read()

if output_result == expected_result:
    exit(0)
else:
    if len(output_result) != len(expected_result):
        print('Size mismatch:', len(output_result), 'versus', len(expected_result))
    for i in range(min(len(output_result), len(expected_result))):
        if(output_result[i] != expected_result[i]):
            print('First mismatch at byte', i)
            break
    exit(1)
