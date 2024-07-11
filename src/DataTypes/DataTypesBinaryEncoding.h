#pragma once

#include <DataTypes/IDataType.h>

namespace DB
{

/**

Binary encoding for ClickHouse data types:
|------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ClickHouse data type                                                               | Binary encoding                                                                                                                                                                                                  |
|------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Nothing                                                                            | 0x00                                                                                                                                                                                                             |
| UInt8                                                                              | 0x01                                                                                                                                                                                                             |
| UInt16                                                                             | 0x02                                                                                                                                                                                                             |
| UInt32                                                                             | 0x03                                                                                                                                                                                                             |
| UInt64                                                                             | 0x04                                                                                                                                                                                                             |
| UInt128                                                                            | 0x05                                                                                                                                                                                                             |
| UInt256                                                                            | 0x06                                                                                                                                                                                                             |
| Int8                                                                               | 0x07                                                                                                                                                                                                             |
| Int16                                                                              | 0x08                                                                                                                                                                                                             |
| Int32                                                                              | 0x09                                                                                                                                                                                                             |
| Int64                                                                              | 0x0A                                                                                                                                                                                                             |
| Int128                                                                             | 0x0B                                                                                                                                                                                                             |
| Int256                                                                             | 0x0C                                                                                                                                                                                                             |
| Float32                                                                            | 0x0D                                                                                                                                                                                                             |
| Float64                                                                            | 0x0E                                                                                                                                                                                                             |
| Date                                                                               | 0x0F                                                                                                                                                                                                             |
| Date32                                                                             | 0x10                                                                                                                                                                                                             |
| DateTime                                                                           | 0x11                                                                                                                                                                                                             |
| DateTime(time_zone)                                                                | 0x12<var_uint_time_zone_name_size><time_zone_name_data>                                                                                                                                                          |
| DateTime64(P)                                                                      | 0x13<uint8_precision>                                                                                                                                                                                            |
| DateTime64(P, time_zone)                                                           | 0x14<uint8_precision><var_uint_time_zone_name_size><time_zone_name_data>                                                                                                                                         |
| String                                                                             | 0x15                                                                                                                                                                                                             |
| FixedString(N)                                                                     | 0x16<var_uint_size>                                                                                                                                                                                              |
| Enum8                                                                              | 0x17<var_uint_number_of_elements><var_uint_name_size_1><name_data_1><int8_value_1>...<var_uint_name_size_N><name_data_N><int8_value_N>                                                                           |
| Enum16                                                                             | 0x18<var_uint_number_of_elements><var_uint_name_size_1><name_data_1><int16_little_endian_value_1>...><var_uint_name_size_N><name_data_N><int16_little_endian_value_N>                                            |
| Decimal32(P, S)                                                                    | 0x19<uint8_precision><uint8_scale>                                                                                                                                                                               |
| Decimal64(P, S)                                                                    | 0x1A<uint8_precision><uint8_scale>                                                                                                                                                                               |
| Decimal128(P, S)                                                                   | 0x1B<uint8_precision><uint8_scale>                                                                                                                                                                               |
| Decimal256(P, S)                                                                   | 0x1C<uint8_precision><uint8_scale>                                                                                                                                                                               |
| UUID                                                                               | 0x1D                                                                                                                                                                                                             |
| Array(T)                                                                           | 0x1E<nested_type_encoding>                                                                                                                                                                                       |
| Tuple(T1, ..., TN)                                                                 | 0x1F<var_uint_number_of_elements><nested_type_encoding_1>...<nested_type_encoding_N>                                                                                                                             |
| Tuple(name1 T1, ..., nameN TN)                                                     | 0x20<var_uint_number_of_elements><var_uint_name_size_1><name_data_1><nested_type_encoding_1>...<var_uint_name_size_N><name_data_N><nested_type_encoding_N>                                                       |
| Set                                                                                | 0x21                                                                                                                                                                                                             |
| Interval                                                                           | 0x22<interval_kind>                                                                                                                                                                                              |
| Nullable(T)                                                                        | 0x23<nested_type_encoding>                                                                                                                                                                                       |
| Function                                                                           | 0x24<var_uint_number_of_arguments><argument_type_encoding_1>...<argument_type_encoding_N><return_type_encoding>                                                                                                  |
| AggregateFunction(function_name(param_1, ..., param_N), arg_T1, ..., arg_TN)       | 0x25<var_uint_version><var_uint_function_name_size><function_name_data><var_uint_number_of_parameters><param_1>...<param_N><var_uint_number_of_arguments><argument_type_encoding_1>...<argument_type_encoding_N> |
| LowCardinality(T)                                                                  | 0x26<nested_type_encoding>                                                                                                                                                                                       |
| Map(K, V)                                                                          | 0x27<key_type_encoding><value_type_encoding>                                                                                                                                                                     |
| IPv4                                                                               | 0x28                                                                                                                                                                                                             |
| IPv6                                                                               | 0x29                                                                                                                                                                                                             |
| Variant(T1, ..., TN)                                                               | 0x2A<var_uint_number_of_variants><variant_type_encoding_1>...<variant_type_encoding_N>                                                                                                                           |
| Dynamic(max_types=N)                                                               | 0x2B<uint8_max_types>                                                                                                                                                                                            |
| Custom type (Ring, Polygon, etc)                                                   | 0x2C<var_uint_type_name_size><type_name_data>                                                                                                                                                                    |
| Bool                                                                               | 0x2D                                                                                                                                                                                                             |
| SimpleAggregateFunction(function_name(param_1, ..., param_N), arg_T1, ..., arg_TN) | 0x2E<var_uint_function_name_size><function_name_data><var_uint_number_of_parameters><param_1>...<param_N><var_uint_number_of_arguments><argument_type_encoding_1>...<argument_type_encoding_N>                   |
| Nested(name1 T1, ..., nameN TN)                                                    | 0x2F<var_uint_number_of_elements><var_uint_name_size_1><name_data_1><nested_type_encoding_1>...<var_uint_name_size_N><name_data_N><nested_type_encoding_N>                                                       |
|------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

Interval kind binary encoding:
|---------------|-----------------|
| Interval kind | Binary encoding |
|---------------|-----------------|
| Nanosecond    | 0x00            |
| Microsecond   | 0x01            |
| Millisecond   | 0x02            |
| Second        | 0x03            |
| Minute        | 0x04            |
| Hour          | 0x05            |
| Day           | 0x06            |
| Week          | 0x07            |
| Month         | 0x08            |
| Quarter       | 0x09            |
| Year          | 0x1A            |
|---------------|-----------------|

Aggregate function parameter binary encoding (binary encoding of a Field, see src/Common/FieldBinaryEncoding.h):
|------------------------|------------------------------------------------------------------------------------------------------------------------------|
| Parameter type         | Binary encoding                                                                                                              |
|------------------------|------------------------------------------------------------------------------------------------------------------------------|
| Null                   | 0x00                                                                                                                         |
| UInt64                 | 0x01<var_uint_value>                                                                                                         |
| Int64                  | 0x02<var_int_value>                                                                                                          |
| UInt128                | 0x03<uint128_little_endian_value>                                                                                            |
| Int128                 | 0x04<int128_little_endian_value>                                                                                             |
| UInt128                | 0x05<uint128_little_endian_value>                                                                                            |
| Int128                 | 0x06<int128_little_endian_value>                                                                                             |
| Float64                | 0x07<float64_little_endian_value>                                                                                            |
| Decimal32              | 0x08<var_uint_scale><int32_little_endian_value>                                                                              |
| Decimal64              | 0x09<var_uint_scale><int64_little_endian_value>                                                                              |
| Decimal128             | 0x0A<var_uint_scale><int128_little_endian_value>                                                                             |
| Decimal256             | 0x0B<var_uint_scale><int256_little_endian_value>                                                                             |
| String                 | 0x0C<var_uint_size><data>                                                                                                    |
| Array                  | 0x0D<var_uint_size><value_encoding_1>...<value_encoding_N>                                                                   |
| Tuple                  | 0x0E<var_uint_size><value_encoding_1>...<value_encoding_N>                                                                   |
| Map                    | 0x0F<var_uint_size><key_encoding_1><value_encoding_1>...<key_endoding_N><value_encoding_N>                                   |
| IPv4                   | 0x10<uint32_little_endian_value>                                                                                             |
| IPv6                   | 0x11<uint128_little_endian_value>                                                                                            |
| UUID                   | 0x12<uuid_value>                                                                                                             |
| Bool                   | 0x13<bool_value>                                                                                                             |
| Object                 | 0x14<var_uint_size><var_uint_key_size_1><key_data_1><value_encoding_1>...<var_uint_key_size_N><key_data_N><value_encoding_N> |
| AggregateFunctionState | 0x15<var_uint_name_size><name_data><var_uint_data_size><data>                                                                |
| Negative infinity      | 0xFE                                                                                                                         |
| Positive infinity      | 0xFF                                                                                                                         |
|------------------------|------------------------------------------------------------------------------------------------------------------------------|
*/

String encodeDataType(const DataTypePtr & type);
void encodeDataType(const DataTypePtr & type, WriteBuffer & buf);

DataTypePtr decodeDataType(const String & data);
DataTypePtr decodeDataType(ReadBuffer & buf);

}
