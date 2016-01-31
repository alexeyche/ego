#pragma once

#include "server.h"

#include <ego/model/model.h>
#include <ego/util/json.h>


namespace NEgo {

	class TProblem {
	public:
		TProblem(TString name)
			: Name(name) 
		{
		}

		TString GetName() const {
			return Name;
		}

		// TModel& GetModel() {
		// 	return Model;
		// }

	private:
		TString Name;

		// TModel Model;
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
						TJsonDocument jsonDoc;
						for (const auto& p: Problems) {
							jsonDoc.PushBack(p.first);
						}
						resp.Body() += jsonDoc.GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/submit_problem",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TJsonDocument jsonDoc(req.Body);
						TString name = jsonDoc.Get<TString>("name");

						auto res = Problems.insert(MakePair(name, TProblem(name)));
						if (!res.second) {
							throw TEgoLogicError() << "Problem with the name `" << name << "' is already exist";
						}

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