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


#ifndef INCLUDED_OP25_TPP_DECODER_FF_H
#define INCLUDED_OP25_TPP_DECODER_FF_H

#include <op25/api.h>
#include <gnuradio/block.h>
#include <gnuradio/msg_queue.h>


namespace gr {
namespace op25 {

/*!
 * \brief <+description of block+>
 * \ingroup op25
 *
 */
class OP25_API tpp_decoder_ff : virtual public gr::block
{
public:
	typedef boost::shared_ptr<tpp_decoder_ff> sptr;

	/*!
	 * \brief Return a shared_ptr to a new instance of op25::tpp_decoder_ff.
	 *
	 * To avoid accidental use of raw pointers, op25::tpp_decoder_ff's
	 * constructor is in a private implementation
	 * class. op25::tpp_decoder_ff::make is the public interface for
	 * creating new instances.
	 */
	static sptr make();

};

} // namespace op25
} // namespace gr

#endif /* INCLUDED_OP25_TPP_DECODER_FF_H */

