
#include <cassert>
#include <cstring>
#include <string>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdint>
#include <iostream>
#include <chrono>

/**
 * @brief Class for a Serialisable Element
 *
 * @author Kiran de Silva
 *
 * @tparam C Element container type
 * @tparam T Type of the element
 */
template <class C, class T>
class RnpSerializableElement {

private:
    /**
     * @brief Element size
     */
    static constexpr size_t size = sizeof(T);

    /**
     * @brief Member variable pointer
     *
     * Solution from:
     * https://riptutorial.com/cplusplus/example/6997/pointers-to-member-variables.
     * Allows classes to be static and allows pointers to class members to be
     * defined at compile time.
     */
    T C::*ptr;

public:
    /**
     * @brief Construct a new Serialisable Element object
     *
     * @param[in] elem Class pointer to the element
     */
    constexpr RnpSerializableElement(T C::*elem) : ptr(elem) {} // constructor

    /**
     * @brief Serialize the element
     *
     * @author Kiran de Silva
     *
     * @param[in] owner Reference to the container
     * @param[out] buffer Output buffer
     */
    void serialize(const C &owner, std::vector<uint8_t> &buffer) const {
        // Calculate buffer size
        const size_t bufSize = buffer.size();

        // Resize buffer to add space for the new element
        buffer.resize(bufSize + size);

        // Copy new element onto the end of the buffer
        std::memcpy(buffer.data() + bufSize, &(owner.*ptr), size);
    }

    /**
     * @brief Deserialize the element
     *
     * @author Kiran de Silva
     *
     * @param[out] owner Reference to the container
     * @param[in] buffer Input buffer
     * @param[in] offset Offset in the buffer for the element
     * @return size_t Size of the element
     */
    size_t deserialize(C &owner, const std::vector<uint8_t> &buffer,
                       const size_t offset) const {
        // Check that the size of the buffer is not exceeded
        /// @todo Dump packet instead?
        assert(offset + size <= buffer.size());

        // Copy the element from the buffer
        std::memcpy(&(owner.*ptr), buffer.data() + offset, size);

        // Return the size of the element
        return size;
    }

    /**
     * @brief Convert element to a string
     * 
     * @param owner Reference to the container
     * @param buffer Input buffer
     */
    void stringify(const C& owner, std::stringstream &buffer) const 
    {
        
        buffer << std::to_string(owner.*ptr) << ',';
    }

};

/**
 * @brief Class for a Serializer
 *
 * @author Kiran de Silva
 *
 * @tparam C Element container type
 * @tparam T Type of the element
 */
template <class C, class... T> // variadic template
class RnpSerializer {

private:
    /**
     * @brief Elements for (de)serialization
     *
     * A tuple is used to support elements with different types
     */
    std::tuple<RnpSerializableElement<C, T>...> elements;

    /**
     * @brief Deserialize the buffer
     *
     * @author Kiran de Silva
     *
     * @tparam I Element iteration
     * @param[out] owner Reference to the container
     * @param[in] buffer Input buffer
     * @param[in] pos Element position
     */
    template <size_t I>
    void deserialize_impl(C &owner, const std::vector<uint8_t> &buffer,
                          [[maybe_unused]] const size_t pos) const {
        // Check that iteration is within the size of the buffer
        if constexpr (I < sizeof...(T)) {
            // Get the I-th element from the elements tuple and deserialize
            auto element_size =
                std::get<I>(elements).deserialize(owner, buffer, pos);

            // Recurse by calling the deserialisation of the next element
            deserialize_impl<I + 1>(owner, buffer, pos + element_size);
        }
    }

public:
    /**
     * @brief Construct a new Rnp Serializer object
     *
     * @author Kiran de Silva
     *
     * @param[in] ptrs Member variable pointers
     */
    constexpr RnpSerializer(T C::*...ptrs)
        : elements(std::make_tuple(RnpSerializableElement(ptrs)...)) {}

    /**
     * @brief Calculate the size of all members
     *
     * @author Kiran de Silva
     *
     * @return constexpr size_t Size of all member elements
     */
    static constexpr size_t member_size() {
        // Return the sum of all member sizes
        return (0 + ... + sizeof(T));
    }

    /**
     * @brief Serialize the elements
     *
     * @author Kiran de Silva
     *
     * @param[in] owner Reference to the container
     * @return std::vector<uint8_t> Serialized bytes
     */
    std::vector<uint8_t> serialize(const C &owner) const {
        // Declare buffer for the serialized objects
        std::vector<uint8_t> ret;

        // Allocate memory but do not change the size so we can easily get the
        // end of buffer in serialize
        ret.reserve(member_size());

        // Apply serialise to all of the elements
        std::apply([&](auto &&...args) { (..., args.serialize(owner, ret)); },
                   elements);

        // Return the serialized bytes
        return ret;
    }

    /**
     * @brief Create string-based csv from member values
     * 
     * @param owner 
     * @return std::string 
     */
    std::string stringify(const C& owner) const
    {
        std::stringstream ss; // output string stream

        std::apply(        
            [&](auto&&... args)   
            {
                (..., args.stringify(owner, ss)); //apply stringify on all elements
            },
                elements);

        return ss.str(); // return string bytes
    }

    /**
     * @brief Deserialize the elements
     *
     * @author Kiran de Silva
     *
     * @param[out] owner Reference to container
     * @param[in] buffer Input buffer
     */
    void deserialize(C &owner, const std::vector<uint8_t> &buffer) const {
        // Deserialize the buffer
        deserialize_impl<0>(owner, buffer, 0);
    }
};



class TelemetryLogframe{
private:  
    static constexpr auto getSerializer()
    {
        auto ret = RnpSerializer(
            &TelemetryLogframe::ch0sens,
            &TelemetryLogframe::ch1sens,
            &TelemetryLogframe::ch2sens,
            &TelemetryLogframe::ch3sens,
            &TelemetryLogframe::ch4sens,
            &TelemetryLogframe::ch5sens,
            &TelemetryLogframe::ch6sens,
            &TelemetryLogframe::ch7sens,
            &TelemetryLogframe::ch8sens,
            &TelemetryLogframe::ch9sens,
            &TelemetryLogframe::ch10sens,
            &TelemetryLogframe::ch11sens,
            &TelemetryLogframe::temp0,
            &TelemetryLogframe::temp1,
            &TelemetryLogframe::temp2,
            &TelemetryLogframe::temp3,
            &TelemetryLogframe::timestamp
            
           

        );
        return ret;
    }

public:
    //gps
    float ch0sens,ch1sens,ch2sens,ch3sens,ch4sens,ch5sens;
    float ch6sens,ch7sens,ch8sens,ch9sens,ch10sens,ch11sens;

    float temp0,temp1,temp2,temp3;

    
    uint64_t timestamp;

    std::string stringify()const{
        return getSerializer().stringify(*this) + "\n";
    };

};


std::chrono::milliseconds getmillis(){return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch());};


int main(int argc, char* argv[])
{
    TelemetryLogframe logframe;

    logframe.ch0sens = 1;
    logframe.ch1sens = 2;
    logframe.ch2sens = 3;
    logframe.ch3sens = 4;
    logframe.ch4sens = 5;
    logframe.ch5sens = 6;
    logframe.ch6sens = 7;
    logframe.ch7sens = 8;
    logframe.ch8sens = 9;
    logframe.ch9sens = 10;
    logframe.ch10sens = 11;
    logframe.ch11sens = 12;

    logframe.temp0 = 13;
    logframe.temp1 = 14;
    logframe.temp2 = 15;
    logframe.temp3 = 16;

    logframe.timestamp = 100;

    std::string stringdata;
    
    for (int i_repeat = 0; i_repeat < 100; ++i_repeat)
    {
        stringdata = logframe.stringify();
    }

    std::cout<<stringdata<<std::endl;


    return 0;
}