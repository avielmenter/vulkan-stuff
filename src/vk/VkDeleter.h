#ifndef _VK_DELETER_H_
#define _VK_DELETER_H_

#include <vulkan/vulkan.h>
#include <functional>

template<typename T>
class VkDeleter
{
	private:
		T object{VK_NULL_HANDLE};
		std::function<void(T)> deleter;

		void cleanup()
		{
			if (this->object != VK_NULL_HANDLE)
				this->deleter(object);
			this->object = VK_NULL_HANDLE;
		}

	public:
		VkDeleter() : VkDeleter([](T, VkAllocationCallbacks*) {})
		{
		}

		VkDeleter(std::function<void(T, VkAllocationCallbacks*)> deletef)
		{
			this->deleter = [=](T obj)
			{
				deletef(obj, nullptr);
			};
		}

		VkDeleter(const VkDeleter<VkInstance> &instance, std::function<void(VkInstance, T, VkAllocationCallbacks *)> deletef)
		{
			this->deleter = [&instance, deletef](T obj)
			{
				deletef(instance, obj, nullptr);
			};
		}

		VkDeleter(const VkDeleter<VkDevice> &device, std::function<void(VkDevice, T, VkAllocationCallbacks *)> deletef)
		{
			this->deleter = [&device, deletef](T obj)
			{
				deletef(device, obj, nullptr);
			};
		}

		~VkDeleter()
		{
			this->cleanup();
		}

		T &get()
		{
			return this->object;
		}

		const T &get() const
		{
			return this->object;
		}

		const T *operator &() const
		{
			&object;
		}

		T* replace()
		{
			this->cleanup();
			return &(this->object);
		}

		operator T() const
		{
			return this->object;
		}

		void operator =(T rhs)
		{
			if (rhs == this->object)
				return;

			this->cleanup();
			this->object = rhs;
		}
};

#endif
