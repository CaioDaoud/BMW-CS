aux_source_directory( utils UTILS )
aux_source_directory( parser PARSER )
aux_source_directory( . SRC )
aux_source_directory( ../../indexer/utils UTILS2 )
aux_source_directory( ../../indexer/src SRC2 )


if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library( queryProcessor2 ${UTILS} ${UTILS2} ${SRC2} ${SRC} ${PARSER} )
#target_link_libraries( queryProcessor2 indexer)

add_executable(QueryProcessor run/queryProcessor.cpp)
target_link_libraries (QueryProcessor queryProcessor2 rt)

#add_executable(geraR_Indice run/geraR_Indice.cc)
#target_link_libraries (geraR_Indice indexer)

#add_executable(QueryProcessorHttp run/queryProcessorHttp.cpp)
#target_link_libraries (QueryProcessorHttp queryProcessor2)

#add_executable(QueryProcessorThread run/queryProcessorThread.cpp)
#target_link_libraries (QueryProcessorThread queryProcessor2)

#add_executable(DataSetInfo run/datasetInfo.cpp)
#target_link_libraries (DataSetInfo queryProcessor2)


set( ${PROJECT_NAME}_TO_INSTALL queryProcessor2 )
include ( ${${PROJECT_NAME}_SOURCE_DIR}/install.cmake )

