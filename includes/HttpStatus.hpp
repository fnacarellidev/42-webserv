#pragma once

namespace HttpStatus {
	enum Code {
		OK = 200,
		MOVED_PERMANENTLY = 301,
		NOCONTENT = 204,
		FORBIDDEN = 403,
		NOTFOUND = 404,
		NOTALLOWED = 405,
		SERVERERR = 500
	};
};
