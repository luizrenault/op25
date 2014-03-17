/* -*- c++ -*- */

#define OP25_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "op25_swig_doc.i"

%{
#include "op25/fsk4_slicer.h"
#include "op25/decoder_bf.h"
#include "op25/op25_fsk4_demod_ff.h"
%}


%include "op25/fsk4_slicer.h"
GR_SWIG_BLOCK_MAGIC2(op25, fsk4_slicer);
%include "op25/decoder_bf.h"
GR_SWIG_BLOCK_MAGIC2(op25, decoder_bf);
%include "op25/op25_fsk4_demod_ff.h"
GR_SWIG_BLOCK_MAGIC2(op25, op25_fsk4_demod_ff);
