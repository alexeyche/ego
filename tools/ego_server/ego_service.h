#pragma once

#include "server.h"

#include <ego/model/model.h>
#include <ego/problem/problem.h>
#include <ego/util/json.h>
#include <ego/util/protobuf.h>
#include <ego/base/factory.h>
#include <ego/util/fs.h>
#include <ego/contrib/pbjson/pbjson.h>

namespace NEgo {


	class TEgoService {
	public:
		TEgoService(ui32 port, bool debugMode, TString stateDir)
			: Server(port, 10, debugMode)
			, StateDir(stateDir)
		{
			for (const auto& f: ListDir(StateDir)) {
				TFsPath fullPath = TFsPath(StateDir) / TFsPath(f);
				if (!fullPath.IsFile()) {
					continue;
				}
				L_DEBUG << "Going to deserialize problem from " << fullPath;
				TProblem prob(fullPath);
				auto res = Problems.insert(MakePair(prob.GetName(), prob));
				ENSURE(res.second, "Problem with the name `" << prob.GetName() << "' is already exist");
			}

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
					"POST",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						throw TEgoLogicError() << "Bad request";
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/performance.json",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						const TVectorD& Y = GetProblem(req).GetModel().GetData().second;
						resp.Body() += TJsonDocument::Array(Y).GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/specification.json",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TProblem& prob = GetProblem(req);
						resp.Body() += NPbJson::ProtobufToJson(prob.GetConfig().ProtoConfig);
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/problem/{problem_name}/update_model",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						GetProblem(req).GetModel().Update();
					}
				)

				.AddCallback(
					"POST", "api/problem/{problem_name}/accept",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						GetProblem(req).Accept(TJsonDocument(req.Body));
						resp.Accepted();
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
					"GET", "api/list_model_parts",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TModelConfig config;
						TJsonDocument jsonDoc;

						jsonDoc["Cov"]["Values"] = Factory.GetCovNames();
						jsonDoc["Mean"]["Values"] = Factory.GetMeanNames();
						jsonDoc["Lik"]["Values"] = Factory.GetLikNames();
						jsonDoc["Inf"]["Values"] = Factory.GetInfNames();
						jsonDoc["Acq"]["Values"] = Factory.GetAcqNames();

						jsonDoc["Cov"]["Default"] = config.Cov;
						jsonDoc["Mean"]["Default"] = config.Mean;
						jsonDoc["Lik"]["Default"] = config.Lik;
						jsonDoc["Inf"]["Default"] = config.Inf;
						jsonDoc["Acq"]["Default"] = config.Acq;

						resp.Body() += jsonDoc.GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/submit_problem",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						NEgoProto::TProblemSpec problemSpec;

						L_DEBUG << "Got problem:\n" << req.Body;

						ReadProtoText(req.Body, problemSpec);

					    TProblemConfig problemConf(problemSpec.problemconfig());
					    TModelConfig modelConfig(problemSpec.modelconfig());

						auto res = Problems.insert(
							MakePair(
								problemConf.Name,
								TProblem(problemConf, modelConfig)
							)
						);

						if (!res.second) {
							throw TEgoLogicError() << "Problem with the name `" << problemConf.Name << "' is already exist";
						}

						res.first->second.DumpState(TFsPath(StateDir) / TFsPath(problemConf.Name) + ".pb.txt");

						resp.Body("{}");
						resp.Accepted();
					}
				)
				.MainLoop();
		}

		TProblem& GetProblem(const THttpRequest& req) {
			auto problemNamePtr = req.KeywordsMap.find("problem_name");
			ENSURE(problemNamePtr != req.KeywordsMap.end(), "Keywords for request are not fullfilled");

			TString problemName = problemNamePtr->second;
			auto problemPtr = Problems.find(problemName);
			if (problemPtr == Problems.end()) {
				throw TEgoLogicError() << "Can't find problem with the name " << problemName;
			}

			return problemPtr->second;
		}


	private:
		std::map<TString, TProblem> Problems;

		TServer Server;
		TString StateDir;
	};



} // namespace NEgo