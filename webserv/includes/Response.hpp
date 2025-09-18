/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 12:58:08 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/18 13:32:52 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <iostream>

struct Response {
	std::string	version; //HTTP v1.1
	int 		status; //200, 404, 500
	std::string	reason; //"OK", "Not Found"
	std::map<std::string, std::string> headers; // key/value : "Content-type/length"
	std::string	body; //Body for the HTTP POST mode.

	Response();
};

#endif