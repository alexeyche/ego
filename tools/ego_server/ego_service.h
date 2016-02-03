#pragma once

#include "server.h"

#include <ego/model/model.h>
#include <ego/problem/config.h>
#include <ego/util/json.h>


namespace NEgo {

	
	class TEgoService {
	public:
		TEgoService(ui32 port, bool debugMode)
			: Server(port, 10, debugMode)
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
			
						auto res = Problems.insert(MakePair(name, TProblemConfig()));
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
		std::map<TString, TProblemConfig> Problems;

		TServer Server;
	};



} // namespace NEgo