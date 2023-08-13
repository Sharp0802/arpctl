#ifndef ARPCTL_APPLICATION_H
#define ARPCTL_APPLICATION_H

#include "iapplication.h"
#include "bootstrapper.h"
#include "net/netobj.h"
#include "net/netflow.h"
#include "runopt.h"


class Application final : public IApplication
{
private:
	std::string _device;
	std::shared_ptr<RunOption> _opt;
	std::shared_ptr<Bootstrapper> _bt;
	std::vector<std::shared_ptr<NetworkObject>> _obj;
	std::vector<std::shared_ptr<NetworkFlow>> _flow;

public:
	~Application() override = default;

	bool Configure(const std::vector<std::string>& argv) override;

	void Start() override;

	enum Worker::State Join() override;

	void Abort() override;
};


#endif //ARPCTL_APPLICATION_H
