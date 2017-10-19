//
// Created by windsky on 17-8-14.
//

#ifndef TRASHY_SERVER_STATUS_CODE_H
#define TRASHY_SERVER_STATUS_CODE_H

#include <vector>
#include <string>
#include <utility>

namespace net{


    /**
     * 状态码来自, rfc2616. 较新的文档没找见.....http status code
     * http://www.ietf.org/rfc/rfc2616.txt
     */
    enum STATUS_CODE {
        unknown = 0,
        information_continue = 100,
        information_swith_protocols = 101,
        information_procrssing,

        success_ok = 200,
        success_created,
        success_accepted,
        success_non_authoritative_information,
        success_no_content,
        success_reset_content,
        success_partial_content,

        redirection_multple_choices = 300,
        redirection_moved_permanently,
        redirection_found,
        redirection_see_other,
        redirection_not_modified,
        redirection_use_proxy,
        redirection_unused,
        redirection_temporary_redirect,

        client_error_bad_request = 400,
        client_error_unauthorized,
        client_error_payment_required,
        client_error_forbidden,
        client_error_not_found,
        client_error_method_not_allowed,
        client_error_not_acceptable,
        client_error_proxy_authentication_required,
        client_error_request_timeout,
        client_error_conflict,
        client_error_gone,
        client_error_length_required,
        client_error_precondition_failed,
        client_error_request_entity_too_large,
        client_error_request_uri_too_long,
        client_error_unsupported_media_type,
        client_error_requested_range_not_satisfiable,
        client_error_expectation_failed,

        server_error_internal_server_error = 500,
        server_error_not_implemented,
        server_error_bad_gateway,
        server_error_service_unavailable,
        server_error_gateway_timeout,
        server_error_http_version_not_supported,
    };


    const static std::vector<std::pair<STATUS_CODE, std::string>> status_codes() noexcept
    {
        std::vector<std::pair<STATUS_CODE, std::string>> status_code {
                {STATUS_CODE::unknown, ""},
                {STATUS_CODE::information_continue, "100 Continue"},
                {STATUS_CODE::information_swith_protocols, "101 Switching Protocols"},
                {STATUS_CODE::information_procrssing, "102 Processing"},

                {STATUS_CODE::success_ok, "200 OK"},
                {STATUS_CODE::success_created, "201 Created"},
                {STATUS_CODE::success_accepted, "202 Accepted"},
                {STATUS_CODE::success_non_authoritative_information, "203 Non-Authoritative Information"},
                {STATUS_CODE::success_no_content, "204 No Content"},
                {STATUS_CODE::success_reset_content, "205 Reset Content"},
                {STATUS_CODE::success_partial_content, "206 Partial Content"},

                {STATUS_CODE::redirection_multple_choices, "300 Multiple Choices"},
                {STATUS_CODE::redirection_moved_permanently, "301 Moved Permanently"},
                {STATUS_CODE::redirection_found, "302 Found"},
                {STATUS_CODE::redirection_see_other, "303 See Other"},
                {STATUS_CODE::redirection_not_modified, "304 Not Modified"},
                {STATUS_CODE::redirection_use_proxy, "305 Use Proxy"},
                {STATUS_CODE::redirection_unused, "306 Unused"},
                {STATUS_CODE::redirection_temporary_redirect, "307 Temporary Redirect"},

                {STATUS_CODE::client_error_bad_request, "400 Bad Request"},
                {STATUS_CODE::client_error_unauthorized, "401 Unauthorized"},
                {STATUS_CODE::client_error_payment_required, "402 Payment Required"},
                {STATUS_CODE::client_error_forbidden, "403 Forbidden"},
                {STATUS_CODE::client_error_not_found, "404 Not Found"},
                {STATUS_CODE::client_error_method_not_allowed, "405 Method Not Allowed"},
                {STATUS_CODE::client_error_not_acceptable, "406 Not Acceptable"},
                {STATUS_CODE::client_error_proxy_authentication_required, "407 Proxy Authentication Required"},
                {STATUS_CODE::client_error_request_timeout, "408 Request Timeout"},
                {STATUS_CODE::client_error_conflict, "409 Conflict"},
                {STATUS_CODE::client_error_gone, "410 Gone"},
                {STATUS_CODE::client_error_length_required, "411 Length Required"},
                {STATUS_CODE::client_error_precondition_failed, "412 Precondition Failed"},
                {STATUS_CODE::client_error_request_entity_too_large, "413 Request Entity Too Large"},
                {STATUS_CODE::client_error_request_uri_too_long, "414 Request-URI Too Long"},
                {STATUS_CODE::client_error_unsupported_media_type, "415 Unsupported Media Type"},
                {STATUS_CODE::client_error_requested_range_not_satisfiable, "416 Requested Range Not Satisfiable"},
                {STATUS_CODE::client_error_expectation_failed, "417 Expectation Failed"},

                {STATUS_CODE::server_error_internal_server_error, "500 Internal Server Error"},
                {STATUS_CODE::server_error_not_implemented, "501 Not Implemented"},
                {STATUS_CODE::server_error_bad_gateway, "502 Bad Gateway"},
                {STATUS_CODE::server_error_service_unavailable, "503 Service Unavailable"},
                {STATUS_CODE::server_error_gateway_timeout, "504 Gateway Timeout"},
                {STATUS_CODE::server_error_http_version_not_supported, "505 HTTP Version Not Supported"}
        };
        return status_code;
    }

    inline STATUS_CODE status_code(const std::string &status_str) noexcept
    {
        for(auto &status_code : status_codes()) {
            if(status_code.second == status_str)
                return status_code.first;
        }
        return STATUS_CODE::unknown;
    }


    inline const std::string &status_code(STATUS_CODE code_enum) noexcept
    {
        for(auto &status_code : status_codes()) {
            if(status_code.first == code_enum)
                return status_code.second;
        }
        return status_codes()[0].second;
    }

    inline const char *status_code(size_t code_num) noexcept
    {
        for(auto &status_code : status_codes()) {
            if(status_code.first == code_num)
                return status_code.second.c_str();
        }
        return status_codes()[0].second.c_str();
    }

}   // end of namespace net

#endif //TRASHY_SERVER_STATUS_CODE_H
