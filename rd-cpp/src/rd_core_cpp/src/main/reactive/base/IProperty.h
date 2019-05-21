#ifndef RD_CPP_IPROPERTY_H
#define RD_CPP_IPROPERTY_H

#include "SignalX.h"
#include "Lifetime.h"
#include "IPropertyBase.h"
#include "wrapper.h"

#include <functional>

namespace rd {
	/**
	 * \brief A mutable property.
	 * \tparam T type of stored value (may be abstract)
	 */
	template<typename T>
	class IProperty : public IPropertyBase<T> {
	protected:
		using WT = value_or_wrapper<T>;
	public:

		//region ctor/dtor

		IProperty() = default;

		IProperty(IProperty &&other) = default;

		IProperty &operator=(IProperty &&other) = default;

		explicit IProperty(T const &value) : IPropertyBase<T>(value) {}

		explicit IProperty(T &&value) : IPropertyBase<T>(std::move(value)) {}

		virtual ~IProperty() = default;
		//endregion

		virtual T const &get() const = 0;
	private:
		void advise0(Lifetime lifetime, std::function<void(T const &)> handler, Signal<T> const &signal) const {
			if (lifetime->is_terminated()) {
				return;
			}
			signal.advise(std::move(lifetime), handler);
			if (this->has_value()) {
				handler(this->get());
			}
		}

		void advise_before(Lifetime lifetime, std::function<void(T const &)> handler) const override {
			advise0(lifetime, handler, this->before_change);
		}
	public:
		void advise(Lifetime lifetime, std::function<void(T const &)> handler) const override {
			advise0(std::move(lifetime), std::move(handler), this->change);
		}

		/**
		 * \brief set value of type T or derived type to it.
		 */
		virtual void set(value_or_wrapper<T>) const = 0;

		/**
		 * \brief construct value of type T and delegate call to set
		 */
		template<typename ... Args>
		void emplace(Args &&... args) const {
			set(value_or_wrapper<T>{std::forward<Args>(args)...});
		}
	};
}


#endif //RD_CPP_IPROPERTY_H