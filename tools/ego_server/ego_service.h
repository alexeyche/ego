#pragma once

#include "server.h"

#include <ego/model/model.h>

#include <ego/contrib/rapidjson/writer.h>
#include <ego/contrib/rapidjson/document.h>
#include <ego/contrib/rapidjson/prettywriter.h>

namespace NJson = rapidjson;

namespace NEgo {

	class TProblem {
		TProblem() {
		}

		TString GetName() const {
			return Name;
		}

		TModel& GetModel() {
			return Model;
		}

	private:
		TString Name;

		TModel Model;
	};

	class TEgoService {
	public:
		TEgoService(ui32 port)
			: Server(port)
		{

			Server
				.AddCallback(
					"GET", "/",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						resp.StaticFile("index.html");
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "problem/{problem_name}",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						resp.StaticFile("problem.html");
						resp.Good();
					}
				)
				.AddCallback(
					"GET",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						resp.StaticFile(req.Path);
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/list_problems",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						NJson::Document d;
						NJson::Value array(NJson::kArrayType);
						for (const auto& p: Problems) {
							NJson::Value s;
							s.SetString(p.first.c_str(), p.first.size(), d.GetAllocator());
							array.PushBack(s, d.GetAllocator());
						}

						NJson::Value s;
						s.SetString("Test");
						array.PushBack(s, d.GetAllocator());

						NJson::StringBuffer buffer;
						NJson::PrettyWriter<NJson::StringBuffer> writer(buffer);
						array.Accept(writer);
						resp.Body() = buffer.GetString();
						resp.Body() += "\n";
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/submit_problem",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						L_DEBUG << req.Body;
						resp.Body("{}");
						resp.Accepted();
					}
				)
				.MainLoop();
		}



	private:
		std::map<TString, TProblem> Problems;

		TServer Server;
	};



} // namespace NEgo