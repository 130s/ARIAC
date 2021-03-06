/*
 * Copyright (C) 2016 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef _ARIAC_HH_
#define _ARIAC_HH_

#include <ostream>
#include <map>
#include <string>
#include <vector>

#include <gazebo/gazebo.hh>
#include <ignition/math/Pose3.hh>

namespace ariac
{
  using namespace gazebo;

  typedef std::string KitType_t;
  typedef std::string KittingShipmentType_t;
  typedef std::string AssemblyShipmentType_t;
  typedef std::string OrderID_t;

  /////////////////////////////////////////////////////////////
  /// \brief The score of a kitting shipment.
  /////////////////////////////////////////////////////////////
  class ShipmentScore
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj ShipmentScore object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const ShipmentScore &_obj)
    {
      _out << "kitting_shipment_score" << std::endl;
      _out << "...shipment type: [" << _obj.kittingShipmentType << "]" << std::endl;
      _out << "...completion score: [" << _obj.total() << "]" << std::endl;
      _out << "...complete: [" << (_obj.isComplete ? "true" : "false") << "]" << std::endl;
      _out << "...submitted: [" << (_obj.isSubmitted ? "true" : "false") << "]" << std::endl;
      _out << "...used correct agv: [" << (_obj.correctAGV ? "true" : "false") << "]" << std::endl;
      _out << "...sent to correct station: [" << (_obj.correctDestination ? "true" : "false") << "]" << std::endl;
      _out << "...product type presence score: [" << _obj.productOnlyTypePresence << "]" << std::endl;
      _out << "...product color presence score: [" << _obj.productTypeAndColorPresence << "]" << std::endl;
      _out << "...product pose score: [" << _obj.productPose << "]" << std::endl;
      _out << "...all products bonus: [" << _obj.allProductsBonus << "]" << std::endl;
      // _out << "</kitting_shipment_score>" << std::endl;
      return _out;
    }

  public:
    KittingShipmentType_t kittingShipmentType;
    double productOnlyTypePresence = 0.0;
    double productTypeAndColorPresence = 0.0;
    double allProductsBonus = 0.0;
    double productPose = 0.0;
    bool isComplete = false;  // All products present
    bool isSubmitted = false; // the shipment has been submitted for evaluation
    bool correctAGV = false;  // The shipment was submitted to the desired AGV
    bool correctDestination = false;
    gazebo::common::Time submit_time; // sim time when shipment submitted

    /// \brief Calculate the total score.
    double total() const
    {
      if (!correctAGV)
      {
        return 0.0;
      }
      if (!correctDestination)
      {
        return 0.0;
      }
      return productOnlyTypePresence + productTypeAndColorPresence + allProductsBonus + productPose;
    }
  };

  /////////////////////////////////////////////////////////////
  /// \brief The score of a finished product from assembly.
  /////////////////////////////////////////////////////////////
  class AssemblyScore
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj ShipmentScore object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const AssemblyScore &_obj)
    {
      _out << "<assembly_score " << _obj.assemblyShipmentType << ">" << std::endl;
      _out << "...completion score: [" << _obj.total() << "]" << std::endl;
      _out << "...complete: [" << (_obj.isComplete ? "true" : "false") << "]" << std::endl;
      _out << "...evaluated: [" << (_obj.isEvaluated ? "true" : "false") << "]" << std::endl;
      _out << "...product type presence score: [" << _obj.productOnlyTypePresence << "]" << std::endl;
      _out << "...product color presence score: [" << _obj.productTypeAndColorPresence << "]" << std::endl;
      _out << "...all products bonus: [" << _obj.allProductsBonus << "]" << std::endl;
      _out << "...product pose score: [" << _obj.productPose << "]" << std::endl;
      _out << "...correct assembly station: [" << (_obj.correctStation ? "true" : "false") << "]" << std::endl;
      _out << "</assembly_score>" << std::endl;
      return _out;
    }

  public:
    AssemblyShipmentType_t assemblyShipmentType;
    double productOnlyTypePresence = 0.0;
    double productTypeAndColorPresence = 0.0;
    double allProductsBonus = 0.0;
    double productPose = 0.0;
    bool isComplete = false;          // All products present
    bool isEvaluated = false;         // The finished product has been submitted for evaluation
    bool correctStation = false;      // The finished product was built at the correct station
    gazebo::common::Time submit_time; // Sim time when finished product was submitted

    /// \brief Calculate the total score.
    double total() const
    {
      if (!correctStation)
      {
        return 0.0;
      }
      return productOnlyTypePresence + productTypeAndColorPresence + allProductsBonus + productPose;
    }
  };

  /////////////////////////////////////////////////////////////
  /// \brief The score of an order.
  /// The order score consists of kitting score + assembly score.
  /////////////////////////////////////////////////////////////
  class OrderScore
  {
    //@todo Do the same thing for assembly
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj OrderScore object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const OrderScore &_obj)
    {
      _out << "order score"<< std::endl;
      _out << "...order ID [" << _obj.order_id << "]" << std::endl;
      _out << "...total order score: [" << _obj.computeKittingTotal() << "]" << std::endl;
      _out << "...completion score: [" << _obj.computeKittingCompletionScore() << "]" << std::endl;
      _out << "...time taken: [" << _obj.time_taken << "]" << std::endl;
      _out << "...kitting complete: [" << (_obj.isKittingComplete() ? "true" : "false") << "]" << std::endl;
      _out << "...priority: [" << (_obj.priority) << "]\n";
      for (const auto &item : _obj.kitting_shipment_scores)
      {
        _out << item.second << std::endl;
      }
      return _out;
    }

  public:
    std::string csv_kitting(bool output_header = true) const
    {
      std::stringstream out;
      // Gather data
      size_t kitting_shipments_requested = kitting_shipment_scores.size();
      size_t kitting_shipments_submitted = 0;
      size_t kitting_shipments_completed = 0;
      size_t kitting_shipments_wrong_agv = 0;
      size_t kitting_shipments_wrong_station = 0;
      for (const auto shipment_tuple : kitting_shipment_scores)
      {
        if (shipment_tuple.second.isSubmitted)
        {
          ++kitting_shipments_submitted;
          if (shipment_tuple.second.isComplete)
          {
            ++kitting_shipments_completed;
          }
          if (!shipment_tuple.second.correctAGV)
          {
            ++kitting_shipments_wrong_agv;
          }
          if (!shipment_tuple.second.correctDestination)
          {
            ++kitting_shipments_wrong_station;
          }
        }
      }
      // Output header
      if (output_header)
      {
        out << "\"order_id\",\"completion_score\",\"priority\",\"time\",\"is_complete\","
            << "\"shipments_requested\",\"shipments_submitted\",\"shipments_completed\","
            << "\"shipments_wrong_agv\", \"shipments_wrong_station\"\r\n";
      }
      // Output data
      out << "\"" << order_id << "\",";
      out << computeKittingCompletionScore() << ",";
      out << priority << ",";
      out << time_taken << ",";
      out << (isKittingComplete() ? "1" : "0") << ",";
      out << kitting_shipments_requested << ",";
      out << kitting_shipments_submitted << ",";
      out << kitting_shipments_completed << ",";
      out << kitting_shipments_wrong_agv << ",";
      out << kitting_shipments_wrong_station << "\r\n";
      return out.str();
    }

    /// \brief Mapping between shipment IDs and scores.
  public:
    std::map<KittingShipmentType_t, ShipmentScore> kitting_shipment_scores;
    /// \brief ID of the order being scored.
    OrderID_t order_id;
    /// \brief Time in seconds spend on the order.
    double time_taken = 0.0;
    /// \brief Factor indicating order priority (Allowed values 1 or 3)
    int priority = 1;

    bool has_kitting_task = false;
    bool has_assembly_task = false;

    /// \brief simulation time when order was started
    gazebo::common::Time start_time;

    /// \brief Calculate if the order is complete.
    /// \return True if all shipping boxes have been submitted.
    ///   Will return false if there are no shipping boxes in the order.
    bool isKittingComplete() const
    {
      for (const auto &item : this->kitting_shipment_scores)
      {
        if (!item.second.isSubmitted)
        {
          return false;
        }
      }
      return true;
    };

    /// \brief Calculate the total score.
    double computeKittingTotal() const
    {
      return computeKittingCompletionScore() * priority;
    };

    /// \brief Get score without priority factor.
    double computeKittingCompletionScore() const
    {
      double total = 0.0;
      for (const auto &item : this->kitting_shipment_scores)
      {
        total += item.second.total();
      }
      return total;
    };
  };
  //end class OrderScore

  /////////////////////////////////////////////////////////////
  /// \brief The score of a competition run.
  /////////////////////////////////////////////////////////////
  class GameScore
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj GameScore object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const GameScore &_obj)
    {
      _out << "<game_score>" << std::endl;
      _out << "...total game score: [" << _obj.total() << "]" << std::endl;
      _out << "...total process time: [" << _obj.total_process_time << "]" << std::endl;
      _out << "...arms collision?: [" << _obj.was_arm_arm_collision << "]" << std::endl;
      for (const auto &item : _obj.order_scores_map)
      {
        _out << item.second << std::endl;
      }
      _out << "</game_score>" << std::endl;
      return _out;
    }

  public:
    double total_process_time = 0.0;
    bool was_arm_arm_collision = false;

    // The score of each of the orders during the game.
    std::map<OrderID_t, OrderScore> order_scores_map;

    /// \brief Calculate the total score.
    double total() const
    {
      if (was_arm_arm_collision)
      {
        return 0;
      }
      double total = 0;

      for (const auto &item : this->order_scores_map)
      {
        total += item.second.computeKittingTotal();
      }
      return total;
    };
  };

  /// \brief Determine the model name without namespace
  std::string TrimNamespace(const std::string &modelName)
  {
    // Trim namespaces
    size_t index = modelName.find_last_of('|');
    return modelName.substr(index + 1);
  }

  /// \brief Determine the type of a gazebo model from its name
  std::string DetermineModelType(const std::string &modelName)
  {
    std::string modelType(TrimNamespace(modelName));

    // Trim trailing underscore and number caused by inserting multiple of the same model
    size_t index = modelType.find_last_not_of("0123456789");
    if (modelType[index] == '_' && index > 1)
    {
      modelType = modelType.substr(0, index);
    }

    // Trim "_clone" suffix if exists
    index = modelType.rfind("_clone");
    if (index != std::string::npos)
    {
      modelType.erase(index);
    }

    return modelType;
  }

  /// \brief Determine the ID of a gazebo model from its name
  std::string DetermineModelId(const std::string &modelName)
  {
    std::string modelId(TrimNamespace(modelName));

    // Trim trailing underscore and number caused by inserting multiple of the same model
    size_t index = modelId.find_last_not_of("0123456789");
    if (modelId[index] == '_' && index > 1)
    {
      modelId = modelId.substr(index + 1);
    }

    return modelId;
  }

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about each product contained in a shipment.
  /////////////////////////////////////////////////////////////
  class Product
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj Shipment object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const Product &_obj)
    {
      _out << "  <product>" << std::endl;
      _out << "    <type>" << _obj.type << "" << "</type>" << std::endl;
      _out << "    <faulty>" << (_obj.isFaulty ? "true" : "false") << "</faulty>" << std::endl;
      _out << "    <pose>" << _obj.pose << "" << "</pose>" << std::endl;
      _out << "  </product>" << std::endl;
      return _out;
    }

    /// \brief Product type.
  public:
    std::string type;

    /// \brief Whether or not the product is faulty.
  public:
    bool isFaulty;

    /// \brief Pose in which the product should be placed.
  public:
    ignition::math::Pose3d pose;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about a kitting shipment.
  /////////////////////////////////////////////////////////////
  class KittingShipment
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _shipment shipment to output.
    /// \return The output stream.
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const KittingShipment &_shipment)
    {
      _out << "<kitting_shipment type='" << _shipment.shipment_type << "' agv='" << _shipment.agv_id << "' station='" << _shipment.assembly_station << "'>";
      for (const auto &obj : _shipment.products)
        _out << std::endl<< obj;
      _out << "</kitting_shipment>" << std::endl;

      return _out;
    }

    /// \brief The type of the shipment.
  public:
    KittingShipmentType_t shipment_type;

    /// \brief which AGV this shipment should be put into "agv1", "agv2", "agv3", "agv4", or "any"
  public:
    std::string agv_id;

    /// \brief Station the shipment is sent to: AS1, AS2, AS3, AS4, AS5, or AS6
  public:
    std::string assembly_station;

    /// \brief A shipment is composed of multiple products.
  public:
    std::vector<Product> products;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about an assembly shipment.
  /////////////////////////////////////////////////////////////
  class AssemblyShipment
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _shipment shipment to output.
    /// \return The output stream.
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const AssemblyShipment &_shipment)
    {
      _out << "<assembly_shipment type='" << _shipment.shipmentType << "' station='" << _shipment.assembly_station << "'>";
      for (const auto &obj : _shipment.products)
        _out << std::endl
             << obj;
      _out << "</assembly_shipment>" << std::endl;

      return _out;
    }

    /// \brief The type of the shipment.
  public:
    AssemblyShipmentType_t shipmentType;

    /// \brief Station the shipment is sent to: AS1, AS2, AS3, AS4, AS5, or AS6
  public:
    std::string assembly_station;

    /// \brief A shipment is composed of multiple products.
  public:
    std::vector<Product> products;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about each object contained in a kit.
  /////////////////////////////////////////////////////////////
  class KitObject
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj Kit object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const KitObject &_obj)
    {
      _out << "<object>" << std::endl;
      _out << "...type: [" << _obj.type << "]" << std::endl;
      _out << "...faulty: [" << (_obj.isFaulty ? "true" : "false") << "]" << std::endl;
      _out << "...pose: [" << _obj.pose << "]" << std::endl;
      _out << "</object>" << std::endl;
      return _out;
    }

    /// \brief Object type.
  public:
    std::string type;

    /// \brief Whether or not the object is faulty.
  public:
    bool isFaulty;

    /// \brief Pose in which the object should be placed.
  public:
    ignition::math::Pose3d pose;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about a kit.
  /////////////////////////////////////////////////////////////
  class Kit
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _kit kit to output.
    /// \return The output stream.
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const Kit &_kit)
    {
      _out << "<kit type='" << _kit.kitType << "'>";
      for (const auto &obj : _kit.objects)
        _out << std::endl
             << obj;
      _out << std::endl
           << "</kit>" << std::endl;

      return _out;
    }

    /// \brief The type of the kit.
  public:
    KitType_t kitType;

    /// \brief A kit is composed of multiple objects.
  public:
    std::vector<KitObject> objects;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about each object contained in a briefcase for assembly.
  /////////////////////////////////////////////////////////////
  class AssemblyObject
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _obj Assembly object to output.
    /// \return The output stream
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const AssemblyObject &_obj)
    {
      _out << "<object>" << std::endl;
      _out << "...type: [" << _obj.type << "]" << std::endl;
      _out << "...faulty: [" << (_obj.isFaulty ? "true" : "false") << "]" << std::endl;
      _out << "...pose: [" << _obj.pose << "]" << std::endl;
      _out << "</object>" << std::endl;
      return _out;
    }

    /// \brief Object type.
  public:
    std::string type;

    /// \brief Whether or not the object is faulty.
  public:
    bool isFaulty;

    /// \brief Pose in which the object should be placed.
  public:
    ignition::math::Pose3d pose;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about assembly.
  /////////////////////////////////////////////////////////////
  class Assembly
  {
    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _assembly assembly to output.
    /// \return The output stream.
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const Assembly &_assembly)
    {
      _out << "<assembly type='" << _assembly.assemblyType << "'>";
      for (const auto &obj : _assembly.objects)
        _out << std::endl
             << obj;
      _out << std::endl
           << "</assembly>" << std::endl;

      return _out;
    }

    /// \brief The station where assembly must be performed (AS1, AS2, AS3, AS4, AS5, or AS6)
  public:
    std::string assemblyStation;

    /// \brief The type of the assembly.
  public:
    std::string assemblyType;

    /// \brief Assembly is composed of multiple objects.
  public:
    std::vector<AssemblyObject> objects;
  };

  /////////////////////////////////////////////////////////////
  /// \brief Class to store information about an order.
  /////////////////////////////////////////////////////////////
  class Order
  {
    /// \brief Less than operator.
    /// \param[in] _order Other order to compare.
    /// \return True if this < _order.
  public:
    bool operator<(const Order &_order) const
    {
      return this->start_time < _order.start_time;
    }

    /// \brief Stream insertion operator.
    /// \param[in] _out output stream.
    /// \param[in] _order Order to output.
    /// \return The output stream.
  public:
    friend std::ostream &operator<<(std::ostream &_out,
                                    const Order &_order)
    {
      _out << "<order start_time = " << _order.start_time << ">" << std::endl;
      // _out << "...start time: [" << _order.start_time << "]" << std::endl;

      //--if this order consists of a kitting task then display the shipments
      if (_order.has_kitting_task)
      {
        // _out << "...kitting shipments:" << std::endl;
        for (const auto &item : _order.kitting_shipments)
        {
          _out << item << std::endl;
        }
      }
      //--if this order consists of an assembly task then display the tasks information
      if (_order.has_kitting_task)
      {
        // _out << "...assembly shipments:" << std::endl;
        for (const auto &item : _order.assembly_shipments)
        {
          _out << item << std::endl;
        }
      }
      _out << "</order>" << std::endl;

      return _out;
    }

    /// \brief The ID of this order.
  public:
    OrderID_t order_id;

    /// \brief Simulation time in which the order should be triggered.
  public:
    double start_time;

    /// \brief After how many unwanted products to interrupt the previous order (-1 for never).
  public:
    int interrupt_on_unwanted_products;

    /// \brief After how many wanted products to interrupt the previous order (-1 for never).
  public:
    int interrupt_on_wanted_products;

    /// \brief Which station to send the AGV
  // public:
  //   std::string stationToReach;

    /// \brief Simulation time in seconds permitted for the order to be
    /// completed before cancelling it. Infinite by default.
  public:
    double allowed_time;

    /// \brief An order is composed of multiple shipments of different types.
  public:
    std::vector<KittingShipment> kitting_shipments;

    /// \brief An order is composed of multiple assembly tasks of different types.
  public:
    std::vector<AssemblyShipment> assembly_shipments;

    /// \brief Simulation time in seconds spent on this order.
  public:
    double time_taken;

  public:
    bool has_kitting_task;

  public:
    bool has_assembly_task;
  };
  //-- end class Order

} // namespace ariac
#endif
