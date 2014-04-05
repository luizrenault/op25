/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "tpp_decoder_ff_impl.h"
#include <voice_du_handler.h>
#include <itpp/comm/bch.h>
#include <op25_yank.h>
#include <abstract_data_unit.h>
#include <string>

using namespace std;

namespace gr {
namespace op25 {

class cvsd_decoder : public imbe_decoder {
#define CVSD_SYLLABIC_DECAY(sy_step) sy_step=sy_step*cvsd_syl_decay/1000
#define CVSD_PRINCIPAL_DECAY(pcp_ref) pcp_ref=pcp_ref*cvsd_pcp_decay/1000
#define cvsd_step_min 0
#define cvsd_step_max 400
#define cvsd_companding_mask 0x07
public:
	int32_t cvsd_pcp_decay;
	int32_t cvsd_syl_decay;
	virtual void decode(const voice_codeword& cw){
		cerr<<"Decode CW"<<endl;
	}
	virtual void decode(uint8_t *data, uint16_t size){
		int16_t datad[size*8];
		decode(data,datad, size*8, 1,1);

		audio_samples *samples = audio();
		int en;
		for(en = 0; en <= size*8; en++) {
			float sample = datad[en]/32768.0; //balance v/uv loudness
			samples->push_back(sample);
		}
	}

	cvsd_decoder(uint32_t FreqSample): imbe_decoder(){
		float x, exp;
		x=-200.0/((float)FreqSample);
		exp=1.0 + x + x*x/2.0 + x*x*x/2.0/3.0 + x*x*x*x/4.0/3.0/2.0;
		cvsd_syl_decay=(uint32_t)(exp*1000.0);

		x=-1000.0/((float)FreqSample);
		exp=1.0 + x + x*x/2.0 + x*x*x/2.0/3.0 + x*x*x*x/4.0/3.0/2.0;
		cvsd_pcp_decay=(uint32_t)(exp*1000.0);
	}

	void decode(uint8_t *Source, int16_t *Dest, uint16_t NrSamples, uint8_t NrChannels, uint8_t ActiveChannel){
		static int32_t cvsd_ref=0;
		static int32_t cvsd_step=0;
		static uint8_t companding=0;
		uint8_t nr_bit=0;
		uint16_t i, j;
		for(i=0;i<(NrSamples);i++){
			nr_bit=i%8;

			if((*Source)&(1<<(7-nr_bit))){
				companding|=1;
			}

			if(((companding & cvsd_companding_mask)==cvsd_companding_mask) || ((~companding & cvsd_companding_mask)==cvsd_companding_mask)){
				//increment step size
				cvsd_step+=cvsd_step_max;
			} else {
				//syllabic decay
				CVSD_SYLLABIC_DECAY(cvsd_step);
			}

			//principal decay
			CVSD_PRINCIPAL_DECAY(cvsd_ref);

			if(companding & 1){
				cvsd_ref+=(cvsd_step_min+cvsd_step);
			} else {
				cvsd_ref-=(cvsd_step_min+cvsd_step);
			}
			if(nr_bit==7){
				Source++;
			}
			companding<<=1;

			//		int32_t cvsd_ref1=IIR_Filter(BP_ORDER*2, zeros, polos, x, y, cvsd_ref, 6);
			int32_t cvsd_ref1=cvsd_ref;
			//			int32_t cvsd_ref1=filtro(cvsd_ref);
			//		cvsd_ref1=(int32_t)cvsd_audio_filter((double)cvsd_ref);

			if(cvsd_ref1>32000)
				cvsd_ref1=32000;
			if(cvsd_ref1<-32000)
				cvsd_ref1=-32000;

			if(cvsd_ref>32000)
				cvsd_ref=32000;
			if(cvsd_ref<-32000)
				cvsd_ref=-32000;

			for(j=0;j<NrChannels;j++){
				if(1<<j & ActiveChannel){
					(*(Dest++))=(int16_t)cvsd_ref1;
				} else {
					(*(Dest++))=0;
				}
			}
		}
	}
};


class tpp_data_unit: public abstract_data_unit{
public:
	~tpp_data_unit(){
	}

	tpp_data_unit(const_bit_queue& frame_body) :
		abstract_data_unit(frame_body)
	{
	}

protected:
	virtual std::string duid_str() const
	{
		return std::string("TPP");
	}


	virtual uint16_t frame_size_max() const
	{
		return 32*8+8+16+16;
	}

	virtual void do_correct_errors(bit_vector& frame_body)
	{
	}
	virtual void do_decode_audio(const_bit_vector& frame_body, imbe_decoder& imbe)
	{
		const size_t CAI_SZ = size();
		uint8_t cai[CAI_SZ];
		uint16_t caid[CAI_SZ*8];
		decode_frame(CAI_SZ, cai);
		cvsd_decoder* c = reinterpret_cast<cvsd_decoder*>(&imbe);
		c->decode(cai+5, CAI_SZ-5);
	}



private:
	virtual void extend(dibit d)
	{
		if(frame_size() < frame_size_max()) {
			d_frame_body.push_back(d & 0x1);
		} else {
		}
	}
};




tpp_decoder_ff::sptr tpp_decoder_ff::make()
{
	return gnuradio::get_initial_sptr(new tpp_decoder_ff_impl());
}

/*
 * The private constructor
 */
tpp_decoder_ff_impl::tpp_decoder_ff_impl()
:		gr::block("tpp_decoder_ff",
		gr::io_signature::make(1, 1, sizeof(float)),
		gr::io_signature::make(0, 1, sizeof(float))),
		d_data_unit(),
		d_data_unit_handler(),
		d_frame_hdr(),
		d_imbe(imbe_decoder_sptr(new cvsd_decoder(16000))),
		d_state(SYNCHRONIZING)
{
	d_data_unit_handler = data_unit_handler_sptr(new p25cai_du_handler(d_data_unit_handler, "224.0.0.1", 23456));
	d_data_unit_handler = data_unit_handler_sptr(new voice_du_handler(d_data_unit_handler, d_imbe));
}

/*
 * Our virtual destructor.
 */
tpp_decoder_ff_impl::~tpp_decoder_ff_impl()
{
}

void
tpp_decoder_ff_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
	/* This block consumes 4800 symbols/s and produces 8000
	 * samples/s. That's a work rate of 3/5 or 0.6. If no audio output
	 * is available we'll produce silence.
	 */
	const size_t nof_inputs = ninput_items_required.size();
	const int nof_samples_reqd = .6 * noutput_items;
	fill(&ninput_items_required[0], &ninput_items_required[nof_inputs], nof_samples_reqd);
}

int
tpp_decoder_ff_impl::general_work (int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
{
	const float *in = (const float *) input_items[0];
	float *out = (float *) output_items[0];

	try {

		for(int i = 0; i < ninput_items[0]; ++i) {
			dibit d;
			if(in[i] < 0.0) {
				d = 0;
			} else {
				d = 1;
			} receive_symbol(d);
		}
		consume_each(ninput_items[0]);

		// produce audio
		audio_samples *samples = d_imbe->audio();
		const int n = min(static_cast<int>(samples->size()), noutput_items);
		if(0 < n) {
			copy(samples->begin(), samples->begin() + n, out);
			samples->erase(samples->begin(), samples->begin() + n);
		}
		if(n < noutput_items) {
			fill(out + n, out + noutput_items, 0.0);
		}
		return n;

	} catch(const std::exception& x) {
		cerr << x.what() << endl;
		exit(1);
	} catch(...) {
		cerr << "unhandled exception" << endl;
		exit(2);
	}
}

const bool tpp_decoder_ff_impl::FS[] = {
		0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
		0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, // 0x2DD4
};
const size_t tpp_decoder_ff_impl::FS_SZ = sizeof(FS)/sizeof(FS[0]);


bool tpp_decoder_ff_impl::correlated()
{
	uint8_t errs = 0;
	for(size_t i = 0; i < FS_SZ; ++i) {
		if(d_frame_hdr[i] ^ FS[i]) {
			++errs;
		}
	}
	return (errs <= 0);
}

data_unit_sptr tpp_decoder_ff_impl::identified()
{
	uint8_t duid = extract(d_frame_hdr, FS_SZ, FS_SZ+4);

	if(duid==0) {
		d_data_unit = data_unit_sptr(new tpp_data_unit(d_frame_hdr));
	} else {
		data_unit_sptr null;
		d_data_unit = null;
	}
	return d_data_unit;
}

void tpp_decoder_ff_impl::receive_symbol(dibit d)
{
	d_frame_hdr.push_back(d & 0x1);
	const size_t frame_hdr_sz = d_frame_hdr.size();

	switch(d_state) {
	case SYNCHRONIZING:
		if(FS_SZ <= frame_hdr_sz) {
			d_frame_hdr.erase(d_frame_hdr.begin(), d_frame_hdr.begin() + (frame_hdr_sz - FS_SZ));
			if(correlated()) {
				d_state = IDENTIFYING;
				cerr<<"SYNC"<<endl;
			}
		}
		break;
	case IDENTIFYING:
		if((FS_SZ +8)== frame_hdr_sz) {
			if(identified()) {
				d_state = READING;
				cerr<<"IDENT"<<endl;
			} else {
				d_state = SYNCHRONIZING;
				cerr<<"NOT IDENT"<<endl;
			}
		}
		break;
	case READING:
		d_data_unit->extend(d);
		if(d_data_unit->is_complete()) {
			d_data_unit->correct_errors();
			d_data_unit_handler->handle(d_data_unit);
			data_unit_sptr null;
			d_data_unit = null;
			d_state = SYNCHRONIZING;
			cerr<<"READ"<<endl;
		}
		break;
	}
}


} /* namespace op25 */
} /* namespace gr */

