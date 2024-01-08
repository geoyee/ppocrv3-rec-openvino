CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(openvino_test)
SET(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(OpenCV REQUIRED)

SET(openvino_LIBRARIES "/opt/intel/openvino_2023.2.0/runtime/lib/intel64/libopenvino.so")

INCLUDE_DIRECTORIES(
    include
    /opt/intel/openvino_2023.2.0/runtime/include
    /opt/intel/openvino_2023.2.0/runtime/include/ie
    /opt/intel/openvino_2023.2.0/runtime/include/ngraph
    /opt/intel/openvino_2023.2.0/runtime/include/openvino
    ${OpenCV_INCLUDE_DIR}
)

FILE(COPY test.jpg DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(COPY ch_PP-OCRv3_rec_infer.onnx DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(COPY ppocr_keys_v1.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

AUX_SOURCE_DIRECTORY(src SRC)
ADD_EXECUTABLE(openvino_test main.cpp ${SRC})

TARGET_LINK_LIBRARIES(openvino_test ${OpenCV_LIBS} ${openvino_LIBRARIES})