/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:45:07 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/18 13:17:30 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>

/* std::map<string, string> variable ; explanation.
** it tooks 2 strings, the result will be something like that
** headers["Authorization"] = "token"; access in write and read
** is available with operator [""]; : std::cout << header["content"];
** The operator [""] search if it exists, else it doesn't, it create it.
*/


struct Request {
	std::string method; //GET, POST, DELETE
	std::string uri; //ressources path
	std::string version; //ex : (HTTP/1.1)
	std::map<std::string, std::string> headers; //check explanation
	std::string body; // body of the http request POST

	Request();
};

#endif