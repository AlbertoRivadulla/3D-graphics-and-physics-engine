class IValidatable {
public:
    virtual bool isValid() const = 0;
    virtual ~IValidatable() = default;
};
