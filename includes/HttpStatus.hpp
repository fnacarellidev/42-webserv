#pragma once

namespace HttpStatus {
	enum Code {
		OK = 200,
		NOCONTENT = 204,
		MOVED_PERMANENTLY = 301,
		NOCONTENT = 204,
		FORBIDDEN = 403,
		NOTFOUND = 404,
		NOTALLOWED = 405,
		CONFLICT = 409,
		SERVERERR = 500
	};
};
