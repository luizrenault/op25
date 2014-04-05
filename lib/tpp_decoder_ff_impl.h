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

#ifndef INCLUDED_OP25_TPP_DECODER_FF_IMPL_H
#define INCLUDED_OP25_TPP_DECODER_FF_IMPL_H

#include <op25/tpp_decoder_ff.h>
#include <data_unit.h>
#include <data_unit_handler.h>
#include <imbe_decoder.h>
#include <snapshot_du_handler.h>
#include <p25cai_du_handler.h>


namespace gr {
namespace op25 {

class tpp_decoder_ff_impl : public tpp_decoder_ff
{
private:
	// Nothing to declare in this block.

	/**
	 * When d_state == READING the current data unit, otherwise null.
	 */
	data_unit_sptr d_data_unit;

	/**
	 * The head of a chain of data_unit_handler instances.
	 */
	data_unit_handler_sptr d_data_unit_handler;

	/**
	 * A bit_queue used to correlate the FS.
	 */
	bit_queue d_frame_hdr;

	/**
	 * The IMBE decoder to use.
	 */
	imbe_decoder_sptr d_imbe;

	/**
	 * Valid states for the decoder state model.
	 */
	enum { SYNCHRONIZING, IDENTIFYING, READING } d_state;

	static const size_t FS_SZ;
	static const bool FS[];


public:
	tpp_decoder_ff_impl();
	~tpp_decoder_ff_impl();

	// Where all the action really happens
	void forecast (int noutput_items, gr_vector_int &ninput_items_required);

	int general_work(int noutput_items, gr_vector_int &ninput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);

	/**
	 * Tests whether d_frame_header correlates with the APCO P25 frame
	 * sync sequence. This method must only be called when the frame
	 * header is larger than 48 bits in length (the minimum size for
	 * the FS).
	 *
	 * \return true if the frame header correlates; otherwise false.
	 */
	bool correlated();

	/**
	 * Tests whether d_frame_header identifies a known data unit and if
	 * so sets d_data_unit to point to an appropriate instance and
	 * returns a pointer to it. This method must only be called when
	 * the frame header is larger than 114 bits in length (the minimum
	 * size for a frame containing a NID).
	 *
	 * \return A data_unit_sptr pointing to an appropriate data_unit
	 * instance or NULL if the frame header is unrecognized.
	 */
	data_unit_sptr identified();

	/**
	 * Handle a received symbol.
	 *
	 * \param d The symbol to process.
	 */
	void receive_symbol(dibit d);

};

} // namespace op25
} // namespace gr

#endif /* INCLUDED_OP25_TPP_DECODER_FF_IMPL_H */

