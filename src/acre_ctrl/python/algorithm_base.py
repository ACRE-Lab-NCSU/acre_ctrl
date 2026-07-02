# acre_ctrl/algorithm/algorithm_base.py
from abc import ABC, abstractmethod
from .component_registry import ComponentRegistry
from .components import Action

class Algorithm(ABC):
    def init(self) -> None:
        pass

    @abstractmethod
    def compute(self, world: ComponentRegistry) -> Action:
        ...

    def components(self) -> list[str]:
        return ["odom", "goal"]