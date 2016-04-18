#pragma once

#include <ego/util/server/server.h>

#include <ego/model/model.h>
#include <ego/solver/solver.h>
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
			if (!DirExists(StateDir)) {
				CreateDir(StateDir);
			}
			for (const auto& f: ListDir(StateDir)) {
				TFsPath fullPath = TFsPath(StateDir) / TFsPath(f);
				if (!fullPath.IsFile()) {
					continue;
				}
				L_DEBUG << "Going to deserialize problem from " << fullPath;
				TSolver prob(fullPath);
				auto res = ProblemSolvers.insert(MakePair(prob.GetProblemName(), prob));
				ENSURE(res.second, "Problem with the name `" << prob.GetProblemName() << "' is already exist");
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
						GetProblemSolver(req); // Check if we have that problem
						resp.StaticFile("problem.html");
						resp.Good();
					}
				)
				.AddCallback(
					"POST",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						throw TErrLogicError() << "Bad request";
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/performance",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						const TVectorD& Y = GetProblemSolver(req).GetModel()->GetData().second;
						resp.Body() += TJsonDocument::Array(Y).GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/data.ssv",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						const auto& data = GetProblemSolver(req).GetModel()->GetData();
						TMatrixD X = data.first;
						X.resize(X.n_rows, X.n_cols+1);
						X.col(X.n_cols-1) = data.second;
						resp.Body() += NLa::SepValuesFormat(X, " ");
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/specification",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TSolver& solver = GetProblemSolver(req);
						TString json = NPbJson::ProtobufToJson(solver.GetProblem().GetConfig().Serialize());
						resp.Body() += json;
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/state",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TSolver& probSolver = GetProblemSolver(req);
				        resp.Body() += ProtoTextToString(probSolver.Serialize());
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/variable_slice",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						resp.Body() += GetProblemSolver(req).GetVariableSlice<TJsonDocument>(
							FindUrlArg<TString>(req, "variable_name"),
							FindUrlArg<ui32>(req, "grid_size", 1000)
						).GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"GET", "api/problem/{problem_name}/next_point",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						resp.Body() += GetProblemSolver(req).GetNextPoint<TJsonDocument>().GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/problem/{problem_name}/update_model",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						GetProblemSolver(req).GetModel()->Update();
						resp.Accepted();
					}
				)
				.AddCallback(
					"POST", "api/problem/{problem_name}/optimize_hypers",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						TSolver& solver = GetProblemSolver(req);
						TOptConfig optConfig = solver.GetConfig().HyperOpt;
						optConfig.Method = FindUrlArg<TString>(req, "method", optConfig.Method);
						solver.GetModel()->OptimizeHypers(optConfig);
						resp.Accepted();
					}
				)
				.AddCallback(
					"POST", "api/problem/{problem_name}/add_point",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						auto& p = GetProblemSolver(req);
						p.AddPoint(TJsonDocument(req.Body));
						// SaveProblem(p);
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
						TJsonDocument jsonDoc = TJsonDocument::Array();

						for (const auto& p: ProblemSolvers) {
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
						TSolverConfig solverConfig;
						TJsonDocument jsonDoc;

						jsonDoc["Cov"]["Values"] = Factory.GetCovNames();
						jsonDoc["Mean"]["Values"] = Factory.GetMeanNames();
						jsonDoc["Lik"]["Values"] = Factory.GetLikNames();
						jsonDoc["Inf"]["Values"] = Factory.GetInfNames();
						jsonDoc["Acq"]["Values"] = Factory.GetAcqNames();
						jsonDoc["BatchPolicy"]["Values"] = Factory.GetBatchPolicyNames();
						jsonDoc["ModelType"]["Values"] = Factory.GetModelNames();

						jsonDoc["Cov"]["Default"] = config.Cov;
						jsonDoc["Mean"]["Default"] = config.Mean;
						jsonDoc["Lik"]["Default"] = config.Lik;
						jsonDoc["Inf"]["Default"] = config.Inf;
						jsonDoc["Acq"]["Default"] = config.Acq;
						jsonDoc["BatchPolicy"]["Default"] = solverConfig.BatchPolicy;
						jsonDoc["ModelType"]["Default"] = solverConfig.ModelType;

						resp.Body() += jsonDoc.GetPrettyString();
						resp.Good();
					}
				)
				.AddCallback(
					"POST", "api/submit_problem",
					[&](const THttpRequest& req, TResponseBuilder& resp) {
						NEgoProto::TSolverSpec solverSpecProto;

						L_DEBUG << "Got problem:\n" << req.Body;

						ReadProtoText(req.Body, solverSpecProto);
						TSolverSpec solverSpec(solverSpecProto);

					    auto res = ProblemSolvers.insert(
							MakePair(
								solverSpec.ProblemConfig.Name,
								TSolver(solverSpec)
							)
						);

						if (!res.second) {
							throw TErrLogicError() << "Problem with the name `" << solverSpec.ProblemConfig.Name << "' is already exist";
						}

						SaveProblemSolver(res.first->second);

						resp.Body("{}");
						resp.Accepted();
					}
				)
				.MainLoop();
		}

		void SaveProblemSolver(TSolver& p) {
			p.DumpState(TFsPath(StateDir) / TFsPath(p.GetProblemName()) + ".pb.txt");
		}

		TSolver& GetProblemSolver(const THttpRequest& req) {
			auto problemNamePtr = req.KeywordsMap.find("problem_name");
			ENSURE(problemNamePtr != req.KeywordsMap.end(), "Keywords for request are not fullfilled");

			TString problemName = problemNamePtr->second;
			auto problemPtr = ProblemSolvers.find(problemName);
			if (problemPtr == ProblemSolvers.end()) {
				throw TErrElementNotFound() << "Can't find problem with the name " << problemName;
			}

			return problemPtr->second;
		}

	private:
		std::map<TString, TSolver> ProblemSolvers;

		TServer Server;
		TString StateDir;
	};



} // namespace NEgo